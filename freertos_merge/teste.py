import cv2
import serial
from cvzone.HandTrackingModule import HandDetector

conectado = False
porta = 'COM3'
velocidadeBaud = 115200

try: 
    SerialArduino = serial.Serial(porta, velocidadeBaud, timeout=0.2)
    conectado = True
except serial.SerialException: 
    print("Erro ao conectar. Verifique a porta serial ou religue o Arduino")

# Inicializa a captura de vídeo (use 0 se 1 não funcionar)
video = cv2.VideoCapture(0)
if not video.isOpened():
    print("Erro ao acessar a câmera.")
    exit()

detector = HandDetector(detectionCon=0.7)
while True:
    check, img = video.read()
    if not check:
        print("Erro ao capturar a imagem da câmera.")
        continue

    hands, img = detector.findHands(img)  # Processa a imagem
    if hands:
        fingers = detector.fingersUp(hands[0])
        if fingers == [0, 0, 0, 0, 0]:  
            SerialArduino.write('0\n'.encode())
        elif fingers == [1, 0, 0, 0, 0]:  
            SerialArduino.write('2\n'.encode())
        elif fingers == [1, 1, 0, 0, 0]:  
            SerialArduino.write('3\n'.encode())
        elif fingers == [1, 1, 1, 0, 0]:  
            SerialArduino.write('4\n'.encode())
        elif fingers == [1, 1, 1, 1, 0]:  
            SerialArduino.write('5\n'.encode())
        elif fingers == [1, 1, 1, 1, 1]:  
            SerialArduino.write('6\n'.encode())

    cv2.imshow('IMG', img)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Libera os recursos após sair do loop
video.release()
cv2.destroyAllWindows()
