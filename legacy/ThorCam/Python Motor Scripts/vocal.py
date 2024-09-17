import speech_recognition as sr  
   
# obtain audio from the microphone  
r = sr.Recognizer()  

with sr.Microphone() as source:  
    print("Please wait. Calibrating microphone...")  
    # listen for 5 seconds and create the ambient noise energy level  
    r.adjust_for_ambient_noise(source, duration=5) 
    while True:
        print("Say something!")  
        audio = r.listen(source,phrase_time_limit=1.5)  
    
        # recognize speech using Sphinx  
        try:  
            print("Vosk thinks you said '" + r.recognize_vosk(audio) + "'")  
        except sr.UnknownValueError:  
            print("Vosk could not understand audio")  
        except sr.RequestError as e:  
            print("Vosk error; {0}".format(e))