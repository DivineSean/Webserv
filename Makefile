up:
	docker compose up -d

bash:
	docker compose up -d
	docker compose exec webserv bash

re:
	docker compose up -d --build

down:
	docker compose down

logs:
	docker compose logs -f

clean:
	docker compose down --rmi all --volumes --remove-orphans

.PHONY: up bash re down logs clean
