# ============================================================================
#  Webserv — dev container control (workspace root)
# ----------------------------------------------------------------------------
#  This Makefile manages the Linux dev container. The server itself is built
#  and run from the ./webserv folder (which has its own Makefile), inside the
#  container.
#
#  Typical flow:
#      make up            # build the image and start the container
#      make bash          # enter it; you land in /webserv (the app)
#          make re && ./webserv config/default.cfg
#      make down          # stop and remove the container
# ============================================================================

.PHONY: up bash down logs

up:
	docker compose up -d --build

bash: up
	docker compose exec webserv bash

down:
	docker compose down

logs:
	docker compose logs -f
