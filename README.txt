RUNNING WINDOWS:

Arrow Keys: Move blob
E/Right click: to create blob
Q/A: Height change
W/S: Width change
Mouse Functional

Using Visual studio 2015, I set up openGL using the link provided in the announcement page and follow the video (download C/C++ if using visual studio express by attempting to create a new project in C/C++):

https://www.youtube.com/watch?v=8p76pJsUP44

Then,

Right click project > Properties > Linker > SubSystem > Console

OR

Simply download from the link provided in the youtube video description or use the files provided (install C/C++ on visual studio):

Drag freeglut.dll and glew32.dll into the project folder

Open visual studio and the project then:

Right click project > Properties > C/C++ > General > Additional Include Directories > Add ...\glew32\Include & ...\freeglut\Include

Then,

Linker > General > Additional Library Directories > Add ...\glew32\lib & ...\freeglut\lib

Linker > Input > Additional Dependencies > Add freeglut.lib & glew32.lib

Linker > SubSystem > Console