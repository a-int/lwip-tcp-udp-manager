# LwIP TCP/UDP manager
The TCP/UDP manager for stm32F4 is based on LwIP RAW with use of httpd. It is the web page where the user can easily setup stm32 mode e.g. TCP Server/Client, UDP and send message from stm32 to the end point.

# Details
Internet technologies play a crucial role in the modern world. And to make it easier to develop software using the Internet, there are open source libraries. One of the most popular libraries for embedded devices like STM32F407 is LwIP. LwIP provides API to quickly establish TCP/UDP communication and operate over the Internet.

LwIP TCP/UDP Manager is the project designed to help understand how LwIP can be used to do more than just basic connectivity. Through a web page the user can easily manage different connections with different users and send and receive data from each of them. One of its uses is to show new users how to write more complex projects using HTTP web pages.

The project itself is tested on an stm32F407 with an ethernet connection via a lan8720 module from Wavehare. Built with Make and tested in GDB using Wireshark software. For external client/server, software such as Hercules HW (for Windows) or Network Debugger (for Mac) has been used.

# Build
1. Make sure you have installed arm toolchain and CubeMX;
2. Open .ioc file in CubeMX and select Build system;
3. Open LwIP setting and select IP and Gateway addresses;
4. Build the project;
5. Open web browser and paste selected IP address from step 3;
6. Play

# Demonstration

<img src="/pics/tcpServer.png" width="613" height="372"/>
<img src="/pics/tcpClient.png" width="516" height="436"/>
<img src="/pics/webpage.png" width="410" height="545"/>
