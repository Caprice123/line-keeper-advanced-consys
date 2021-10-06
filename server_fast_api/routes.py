from fastapi.datastructures import UploadFile
from server_fast_api import app
from fastapi import File, UploadFile
import cv2


@app.get("/")
def index():
    return {"name": "First data"}

@app.post("/upload")
async def upload(file: UploadFile = File(...)):
    
    content = await file.read() # bytes
    
    # f = open(f"fast_api_imgs/{file.filename}", "wb")
    # f.write(content)
    # f.close()
    
    
    
    with open(f"fast_api_imgs/{file.filename}", "wb") as f:
        f.write(content)
    
    img = cv2.imread(f"fast_api_imgs/{file.filename}")
    print(img)
    
    return {"filename": file.filename}



