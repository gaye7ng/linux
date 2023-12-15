import pymysql
import pandas as pd
import matplotlib.pyplot as plt

# MySQL 연결 처리
myMyConn = pymysql.connect(user='gayeong', password='young', host='localhost', port=3306, charset='utf8', database='mydb')
myMyCursor = myMyConn.cursor()

# 실행할 SQL 생성
sqlt = """
        SELECT  sensorid, reading, timestamp
        FROM    mydb.SensorData
        WHERE   sensorid=1;
        """
sqlh = """
        SELECT  sensorid, reading, timestamp
        FROM    mydb.SensorData
        WHERE   sensorid=2;
        """

temp = pd.read_sql(sqlt, myMyConn)
humi = pd.read_sql(sqlh, myMyConn)

print(temp)
print(humi)

plt.figure()
plt.plot(temp['timestamp'], temp['reading'], marker='o', linestyle='-', color='red', label='Temperature')
plt.plot(humi['timestamp'], humi['reading'], marker='o', linestyle='-', color='blue', label='Humidity')
plt.xlabel('Timestamp')
plt.ylabel()
plt.savefig('./plot.png')
plt.show()