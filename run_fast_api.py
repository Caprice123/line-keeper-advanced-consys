from server_fast_api import app
import uvicorn

if __name__ == "__main__":
    uvicorn.run(app = app, host="192.168.1.11", port=80, debug=True)