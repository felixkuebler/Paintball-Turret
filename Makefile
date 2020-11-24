MAKE_DIR = $(shell pwd)
AUTO_DIR = /home/pi/.config/autostart/
EXEC_DIR = $(AUTO_DIR)/start.sh

TAG ?= version:1.0
NAME ?= turretServer
PORT ?= 5000

all: build run

# Build Docker File
build:
	docker build --tag $(TAG) .

# Run Docker File (--detach run in background)
run:
	docker run -p $(PORT):$(PORT) --device /dev/video0:/dev/video0 --device /dev/ttyUSB0:/dev/ttyUSB0 --device /dev/ttyUSB1:/dev/ttyUSB1 --name $(NAME) $(TAG)

# Kill current docker container
kill:
	docker kill $(NAME)

clean:
	docker rm $(NAME)

reset:
	docker image prune -f

install:
	cp $(MAKE_DIR)/Utils/autostart-turret.desktop $(AUTO_DIR)
	sed -i 's/{{EXEC_DIR}}/$(MAKE_DIR)/g' $(AUTO_DIR)/autostart-turret.desktop

	cp $(MAKE_DIR)/Utils/start.sh $(MAKE_DIR)
	chmod +x $(MAKE_DIR)/start.sh

info:
	docker ps
	@echo ""
	@echo "==========================================="
	@echo ""
	ps aux | grep docker
