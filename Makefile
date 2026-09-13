up:
	docker compose up -d --build

bash: up
	docker compose exec webserv bash

down:
	docker compose down

logs:
	docker compose logs -f

.PHONY: up bash down logs
