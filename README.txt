
running and building this should be easy, just go onto o
ne of the dcXX dockers and navigate to the directory you
placed it at and run ./build.sh

running it is also easy, do ./launcher.sh as detailed in the misc scripts
just make sure to use the launcher script that came with this project, i modified it
also just make sure to change the CONFIGLOCAL to match the CONFIGDOCKER if you run it on the docker

finally to use the checker, do ./build/Checker <config file name>

it will print the contents of each file, followed by all n^2 compairisons 
between all vector clocks from each timestamp to ensure concurrency
