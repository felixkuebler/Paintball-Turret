MAKE_DIR = $(shell pwd)

UTILS_DIR = Utils
AUTO_DIR = /home/pi/.config/autostart

AUTO_FILE = autostart-turret.desktop
EXEC_FILE = start.sh

TAG ?= version:1.0
NAME ?= turretServer

INTERNAL_PORT ?= 5000
EXTERNAL_PORT ?= 80

all: build run

# Build Docker File
build:
	docker build --tag $(TAG) .

# Run Docker File (--detach run in background)
run:
	docker run -p $(EXTERNAL_PORT):$(INTERNAL_PORT) --device /dev/video0:/dev/video0 --device /dev/ttyUSB0:/dev/ttyUSB0 --device /dev/ttyUSB1:/dev/ttyUSB1 --name $(NAME) $(TAG)

# Kill current docker container
kill:
	docker kill $(NAME)

clean:
	docker rm $(NAME)

reset:
	docker image prune -f

install:
	# copy autostart .desktop to home directory
	cp $(MAKE_DIR)/$(UTILS_DIR)/$(AUTO_FILE) $(AUTO_DIR)
	sed -i 's+{{EXEC_DIR}}+$(MAKE_DIR)/$(EXEC_FILE)+g' $(AUTO_DIR)/$(AUTO_FILE)

	# add autostart command to rc.local
	 sed -i 's+$(MAKE_DIR)/$(EXEC_FILE) &++g' /etc/rc.local
	 sed -i 's+exit 0+$(MAKE_DIR)/$(EXEC_FILE) \&\n\nexit 0+g' /etc/rc.local

	# copy start script to root and make executable
	cp $(MAKE_DIR)/$(UTILS_DIR)/$(EXEC_FILE) $(MAKE_DIR)
	chmod +x $(MAKE_DIR)/$(EXEC_FILE)

	#sudo hostname -b $(NAME)

info:
	docker ps
	@echo ""
	@echo "==========================================="
	@echo ""
	ps aux | grep docker
