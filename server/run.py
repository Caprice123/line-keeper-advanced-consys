from app import app 


if __name__ == "__main__":
    app.run( host="192.168.1.11", port=80, threaded=True, debug=True)
    