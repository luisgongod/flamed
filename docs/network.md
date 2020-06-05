### Network commands

Install the net tools:
```sudo apt install net-tools```

Get the IP range/subnet mask (if needed)
```ipconfig```

Scanning IP addreses
```nmap -sP 192.168.2.0/24```

Scanning with All ip info (names), usefull
```sudo nmap -PU 192.168.2.0/24```

Scanning MACs (short list)
```sudo nmap -sP 192.168.2.0/24 | awk '/Nmap scan report for/{printf $5;}/MAC Address:/{print " => "$3;}' | sort```
