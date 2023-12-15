import pymysql
import pandas as pd
import matplotlib.pyplot as plt
import time

plt.ion()

def update_plot():
    fig, ax = plt.subplots()

    while(1):
        conn = pymysql.connect(user='gayeong', password='young', host='localhost', port=3306, charset='utf8', database='mydb')
        mycursor = conn.cursor()

        sqlt = "SELECT sensor_id, reading, timestamp FROM mydb.SensorData WHERE sensor_id=1"
        sqlh = "SELECT sensor_id, reading, timestamp FROM mydb.SensorData WHERE sensor_id=2"

        temp = pd.read_sql(sqlt, conn)
        humi = pd.read_sql(sqlh, conn)

        print(temp)
        print(humi)

        ax.clear()
        ax.plot(temp['timestamp'], temp['reading'], marker='o', linestyle='-', color='red', label='Temperature')
        ax.plot(humi['timestamp'], humi['reading'], marker='o', linestyle='-', color='blue', label='Humidity')
        plt.xlabel('Timestamp')
        plt.draw()
        plt.pause(10)   # 10초마다 업데이트

try:
    update_plot()
except KeyboardInterrupt:
    pass