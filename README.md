# LineDancer
** Work in progress **

![Image](/img/_2018-02-13-17-48-00_final.gif)


Fast and easy to use drawing program targed for live performances and creating simple frame by frame gif animations.
Build using cinder.

[Example usecase performance](https://vimeo.com/207618159)

OSX binaries can be found here ==>
[Latest releases](https://github.com/lab101/LineDancer/releases)

Keys:
- **v** and **n** to zoom in and out on the Wacom cursor.
- **space** to grab the page.
- **f** to toggle fullscreen.
- **s** to save program settings to settings.json.
- **1** to toggle mouse on and off and disabling the wacom if connected.


Features:
- create gifs by using previous drawing as reference.
- creates gif from used linestrokes.
- zoom, panning.
- drawing in network with friends.
- input by Wacom.

Networking:
By default the software sends all the points over broadcast with OSC.
It will extract the broadcast adres from your subnet.
There's a known issue when your subnet is 255.255.255.0 it will not calculate the correct broadcast adress.