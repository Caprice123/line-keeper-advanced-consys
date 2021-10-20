from flask import request
from app import app
import numpy as np
import cv2
import os
from multiprocessing import Value

# declare counter variable
counter = Value('i', 0)



def save_img(img):
	img_dir = "test_imgs"
 
 
	with counter.get_lock():
	    counter.value += 1
	    count = counter.value
 
	if not os.path.isdir(img_dir):
		os.mkdir(img_dir)
	cv2.imwrite(os.path.join(img_dir,f"img_reg_{count}.jpg"), img) # esp32_imgs/img_reg.jpg
	# print("Image Saved", end="\n") # debug

@app.route('/')
@app.route('/index', methods=['GET'])
def index():
	return "ESP32-CAM Flask Server", 200


@app.route('/upload', methods=['POST','GET'])
def upload():
	received = request
	img = None
	if received.files:
		print(received.files['file'])
		# convert string of image data to uint8
		file  = received.files['file']
		nparr = np.fromstring(file.read(), np.uint8)
		# decode image
		img = cv2.imdecode(nparr, cv2.IMREAD_COLOR)
		save_img(img)

		return "[SUCCESS] Image Received", 201
	else:
		return "[FAILED] Image Not Received", 204
