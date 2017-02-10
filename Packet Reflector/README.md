# Packet Reflector  
Reflects all packets coming from an attacker to a victim back to the attacker using a reflector. The reflector sniffs packets on the victim IP and sends them back to the attacker

## Usage

```
./reflector --interface eth0 --victim-ip 192.168.1.10  --victim-ethernet 31:16:A9:63:FF:83 --reflector-ip 192.168.1.20 --reflector-ethernet 38:45:E3:89:B5:56
```
or 
```
sudo python reflector. py --interface eth0 --victim-ip 192.168.1.10  --victim-ethernet 31:16:A9:63:FF:83 --reflector-ip 192.168.1.20 --reflector-ethernet 38:45:E3:89:B5:56
```
