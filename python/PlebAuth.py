from platform import system
from sys import exit
import subprocess
import uuid
import os
import requests

def auth() -> None:
    if system().lower() == 'windows':
        hwid =  subprocess.check_output('wmic csproduct get uuid').decode()
        hwid = hwid.split('\n')[1].strip()
    elif system().lower() == 'linux':
        hwid = subprocess.check_output(['cat', '/var/lib/dbus/machine-id'])
        hwid = str(uuid.uuid3(uuid.NAMESPACE_DNS, hwid.strip().decode()))
    else:
        hwid = str(uuid.getnode())
    
    if os.path.isfile('key.txt'):
        with open('key.txt') as f:
            auth_key = f.read().strip()
    else:
        auth_key = input('Enter Cracked.to Auth Key: ')
        with open('key.txt', 'w') as f:
            f.write(auth_key)
        
    data = {
        "a": "auth",
        "k": auth_key,
        "hwid": hwid
    }

    r = requests.post('https://cracked.io/auth.php', data=data)
    if r.ok:
        res = r.json()

        if 'error' in res:
             print(res["error"])
             if (os.path.exists("key.txt")):
                 os.remove("key.txt")
             exit(1)
        elif res['auth']:
            print(f'Auth Granted. Welcome {res["username"]}!')
        else:
            print(res)
            exit(1)
    else:
        print(r.text, r.status_code)
        exit(1)
