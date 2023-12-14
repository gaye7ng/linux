#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <mariadb/mysql.h>
#include <MQTTClient.h>

#define DB_HOST "localhost"
#define DB_USER "gayeong"
#define DB_PASSWORD "young"
#define DB_NAME "mydb"

#define MAX_TEXT 512
#define MQTT_BROKER_ADDRESS "tcp://localhost:1883"
#define MQTT_CLIENT_ID "temperature_sensor_client"
#define MQTT_TOPIC "test/temphumi"
#define MQTT_TOPIC1 "test/temp"
#define MQTT_TOPIC2 "test/humi"

char topics[2][10]={
    "test/temp",
    "test/humi"
};

struct message{
    long msg_type;
    char msg_text[MAX_TEXT];
};

void signalHandler(int signum) {
}


int main() {
    char senval[2][10];
    signal(SIGCHLD, signalHandler);

    int pipefd[2];
    pid_t child_pid;
    struct message msg;

    key_t key;
    int msgid;

    if ((key = ftok(".", 'a')) == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    if ((msgid = msgget(key, 0666 | IPC_CREAT)) == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    if ((child_pid = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) {
        close(pipefd[1]);
        char receivedString[100];

        while (1) {
            if (read(pipefd[0], receivedString, sizeof(receivedString)) > 0) {
                receivedString[strcspn(receivedString, "\n")] = 0;
                printf("Consumer received: %s\n", receivedString);

                char *token = strtok((char *)receivedString, ":");
                int i=0;

                while (token != NULL) {
                    strcpy( senval[i], token);
                    printf("%s\n", token);
                    token = strtok(NULL, ":");
                    i++;
                }

                for(int i=0; i < 2; i++) {
                    saveToMariaDB((i+1), senval[i]);
                    sendToMQTT(topics[i], senval[i]);
                }
            }
        }
        close(pipefd[0]);

    } else {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);

        if (execl("./temperatuer.c", "temperatuer", NULL) == -1) {
            perror("execl");
            exit(EXIT_FAILURE);
        }
        
        close(pipefd[1]);
    }

    return 0;
}

void saveToMariaDB(int senid, const char *value){
    MYSQL *conn=mysql_init(NULL);

    if(conn==NULL){
        fprintf(stderr, "mysql_init() failed\n");
        exit(EXIT_FAILURE);
    }

    if(mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASSWORD, DB_NAME, 0, NULL, 0)==NULL){
        fprintf(stderr, "mysql_real_connect() failed\n");
        mysql_close(conn);
        exit(EXIT_FAILURE);
    }

    char query[256];
    sprintf(query, "INSERT INTO SensorData (sensor_id, reading, timestamp) VALUES (%d, '%s', sysdate())", senid, value);

    if(mysql_query(conn, query)!=0){
        fprintf(stderr, "MariaDB query execution failed: %s\n", mysql_error(conn));
    }

    mysql_close(conn);
}

void sendToMQTT(char *topic, const char *value){
    MQTTClient client;
    MQTTClient_connectOptions conn_opts=MQTTClient_connectOptions_initializer;
    int rc;

    MQTTClinet_create(&client, MQTT_BROKER_ADDRESS, MQTT_CLIENT_ID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval=20;
    conn_opts.cleansession=1;

    if((rc=MQTTClient_conect(client, &conn_opts))!=MQTTCLIENT_SUCCESS){
        fprintf(stderr, "Failed to connect to MQTT broker: %d\n", rc);
        exit(EXIT_FAILURE);
    }

    MQTTClient_message pubmsg=MQTTClient_message_initalizer;
    pubmsg.payload = (void *)value;
    pubmsg.payloadlen = strlen(value);
    pubmsg.qos = 1;
    pubmsg.retained = 0;

    MQTTClient_publishMessage(client, topic, &pubmsg, NULL);
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
}