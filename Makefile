.PHONY: book serve

book:
	mdbook build

serve: book
	mdbook serve
