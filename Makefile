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

info:
	docker ps
	@echo ""
	@echo "==========================================="
	@echo ""
	ps aux | grep docker
