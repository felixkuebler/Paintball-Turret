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
	#docker run -p $(EXTERNAL_PORT):$(INTERNAL_PORT) --name $(NAME) $(TAG)
	docker run -p $(EXTERNAL_PORT):$(INTERNAL_PORT) --device /dev/video0:/dev/video0 --device /dev/video2:/dev/video2 --device /dev/ttyUSB0:/dev/ttyUSB0 --name $(NAME) $(TAG)

# Kill current docker container
kill:
	docker kill $(NAME)

clean:
	docker rm $(NAME)

reset:
	docker image prune -f

setup-requirements:
	# uninstall all conflicting packages
	echo "uninstall all conflicting packages"
	for pkg in docker.io docker-doc docker-compose podman-docker containerd runc; do sudo apt-get remove $pkg; done
	
	# add docker official GPG key
	echo "add docker official GPG key"
	sudo apt-get update
	sudo apt-get install ca-certificates curl
	sudo install -m 0755 -d /etc/apt/keyrings
	sudo curl -fsSL https://download.docker.com/linux/debian/gpg -o /etc/apt/keyrings/docker.asc
	sudo chmod a+r /etc/apt/keyrings/docker.asc

	# add the repository to apt sources
	echo \
	  "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.asc] https://download.docker.com/linux/debian \
	  $(. /etc/os-release && echo "$VERSION_CODENAME") stable" | \
	  sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
	sudo apt-get update

	# install docker
	echo "install docker"
	sudo apt-get install docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin

	# verify installation
	echo "verify installation"
	sudo docker run hello-world

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
