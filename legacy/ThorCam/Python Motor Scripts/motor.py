from gpiozero import LED

class Motor:
    def __init__(self,red:int,yellow:int,grey:int,green:int):
        self.posA = LED(green)
        self.negA = LED(yellow) 
        self.posB = LED(red)
        self.negB = LED(grey)
        self.position = 1
    def mvCW(self):
        if(self.position == 4):
            self.position = 1
        else:
            self.position += 1
        if self.position == 1:
            # 0011 -> 1010
            self.posA.on()
            self.negA.off()
            self.posB.on()
            self.negB.off()
        elif self.position == 2:
            # 1010 -> 1100
            self.posA.on()
            self.negA.on()
            self.posB.off()
            self.negB.off()
        elif self.position == 3:
            # 1100 -> 0101
            self.posA.off()
            self.negA.on()
            self.posB.off()
            self.negB.on()
        elif self.position == 4:
            # 0101 -> 0011
            self.posA.off()
            self.negA.off()
            self.posB.on()
            self.negB.on()
        print(self.position)
    def mvCCW(self):
        if(self.position == 1):
            self.position = 4
        else:
            self.position -= 1
        if self.position == 3:
            # 0011 -> 0101
            self.posA.off()
            self.negA.on()
            self.posB.off()
            self.negB.on()
        elif self.position == 2:
            # 0101 -> 1100
            self.posA.on()
            self.negA.on()
            self.posB.off()
            self.negB.off()
        elif self.position == 1:
            # 1100 -> 1010
            self.posA.on()
            self.negA.off()
            self.posB.on()
            self.negB.off()
        elif self.position == 4:
            # 1010 -> 0011
            self.posA.off()
            self.negA.off()
            self.posB.on()
            self.negB.on()
        print(self.position)
    def ResetPins(self):
        self.posA.off()
        self.negA.off()
        self.posB.off()
        self.negB.off()



    