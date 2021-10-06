import cv2

class ImageHandler:
    def __init__(self, img_path):
        self.image = cv2.imread(img_path) 
                
        self.height, self.width, _ = self.image.shape 
        
        self.image_copy = self.image.copy()
        
        
    def denoise(self):
        return cv2.fastNlMeansDenoisingColored(self.image, None, 10, 10, 7, 21)
        
        
app = ImageHandler("img_reg_lurus.jpg")

