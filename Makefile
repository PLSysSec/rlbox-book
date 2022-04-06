.PHONY: serve deploy

serve: book
	mdbook serve

deploy: book
	-git worktree remove /tmp/rlbox-book-gh-pages
	@echo "====> deploying to github"
	git worktree add /tmp/rlbox-book-gh-pages gh-pages
	mdbook build
	rm -rf /tmp/rlbox-book-gh-pages/*
	cp -rp book/* /tmp/rlbox-book-gh-pages/
	cd /tmp/rlbox-book-gh-pages && \
		git update-ref -d refs/heads/gh-pages && \
		git add -A && \
		git commit -m "deployed on $(shell date) by ${USER}" && \
		git push --force origin gh-pages
