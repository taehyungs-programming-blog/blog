# Repository Guidelines

## Project Structure & Module Organization

This repository is a Jekyll documentation/blog site based on the Just the Docs theme. Main content lives in `docs/`, with older migrated material in `docs-history/` excluded from normal builds. Theme templates are in `_layouts/` and `_includes/`, styles are in `_sass/`, and browser assets live under `assets/`. Site-wide settings are in `_config.yml`. Ruby theme support code is in `lib/` and `bin/`; `lib/tasks/search.rake` generates search data. The root `main.cpp`, `CMakeLists.txt`, and `Source/` directory are standalone C++/Unreal examples and should be changed separately from site content.

## Build, Test, and Development Commands

- `bundle install`: install Ruby/Jekyll dependencies from the gemspec.
- `bundle exec jekyll serve`: run the site locally at `http://localhost:4000`.
- `bundle exec jekyll build`: build the static site and catch Liquid/front matter errors.
- `bundle exec rake search:init`: regenerate `assets/js/zzzz-search-data.json` for search.
- `npm install`: install Node tooling from `package-lock.json`.
- `npm test`: run Stylelint across `**/*.scss`.
- `npm run format`: format SCSS, JS, and JSON with Prettier.
- `docker-compose up`: run the Jekyll server in Docker on port `4000`.

## Coding Style & Naming Conventions

Use 2-space indentation for SCSS, JS, JSON, YAML, and Markdown front matter. Prettier is configured with LF endings, no semicolons, double quotes, and trailing commas where valid. Keep SCSS compatible with `stylelint-config-primer`; ignored generated/vendor files are listed in `.stylelintrc.json`. Markdown pages should include clear front matter such as `title`, `parent`, and `nav_order` when they participate in navigation. Prefer existing naming patterns like numbered Markdown files and topic directories under `docs/`.

## Testing Guidelines

There is no separate application test suite. Treat `npm test`, `bundle exec jekyll build`, and `bundle exec rake search:init` as the required validation before opening a PR. For navigation, layout, or asset changes, also inspect the affected pages locally with `bundle exec jekyll serve`.

## Commit & Pull Request Guidelines

Recent commits use short, direct messages such as `graphics pipe` and `shared mem`; keep commits focused on one topic. Pull requests should describe the change, list affected docs or assets, link relevant issues, and include screenshots for visible page or navigation updates. Ensure CI-equivalent checks pass before requesting review.

## Security & Configuration Tips

Do not commit local secrets, generated build output, or dependency directories. When changing `_config.yml`, verify `baseurl`, `url`, analytics, and excluded paths still match the intended GitHub Pages deployment.
