up:
	docker compose up -d

bash:
	docker compose up -d
	docker compose exec webserv bash

down:
	docker compose down

logs:
	docker compose logs -f

clean:
	docker compose down --rmi all --volumes --remove-orphans

re: clean
	docker compose up -d --build

.PHONY: up bash down logs clean re
