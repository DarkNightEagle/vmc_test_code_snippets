from pythonosc.dispatcher import Dispatcher
from pythonosc.osc_server import BlockingOSCUDPServer

def handler(address, *args):
    print(address, args)

disp = Dispatcher()
disp.map("/*", handler)

server = BlockingOSCUDPServer(("0.0.0.0", 8000), disp)
server.serve_forever()
