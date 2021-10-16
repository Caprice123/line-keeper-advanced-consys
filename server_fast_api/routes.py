from fastapi.datastructures import UploadFile
from server_fast_api import app
from fastapi import File, UploadFile
import cv2
from multiprocessing import Value
from fastapi.responses import PlainTextResponse



counter = Value('i', 0)

@app.get("/")
def index():
    return {"name": "First data"}

@app.post("/upload",response_class=PlainTextResponse)
async def upload(file: UploadFile = File(...)):
    
    with counter.get_lock():
	    counter.value += 1
	    count = counter.value
  
    content = await file.read() # bytes
    # print(content)
    with open(f"testing/{file.filename}", "wb") as f:
        f.write(content)
    
    # img = cv2.imread(f"fast_api_imgs/{file.filename}")
    # print(img)
    
    return str(count)



