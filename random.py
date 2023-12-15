import random
from picamera import PiCamera

def camerasys():
    def randnum():
        return random.choice([0, 1])
    
    camera=PiCamera()

    while(1):
        pir=randnum()   # pir 센서
        bt=randnum()    # 버튼

        if(pir==1):  # pir 센서가 사람을 감지하면
            camera.start_recording('./video{}') # 영상 촬영 시작

                if(bt==1):   # 버튼을 누르면
                camera.capture('./photo{}.jpg')   # 사진이 촬영됨
            
            camera.wait_recording(5)
            camera.stop_recording()
        else:
            if(bt==1):   # 버튼을 누르면
                camera.capture('./photo{}.jpg')   # 사진이 촬영됨
    
try:
    camerasys()
except KeyboardInterrupt:
    pass