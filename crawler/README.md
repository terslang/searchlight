# Crawler

The crawler is a web crawler for the Searchlight project. It is responsible for fetching web pages and extracting links from them.

## How it Works

The crawler starts with a set of seed links and then recursively follows the links it finds on those pages. It respects the `robots.txt` file of each website and has a configurable delay between requests to the same host.

### Components

- **LinkManager**: Manages the links to visit, visited links, and the `robots.txt` parsers for each host.
- **WebCrawler**: Fetches the content of a web page and extracts the links from it.
- **RobotsParser**: Parses the `robots.txt` file and provides an interface to check if a URL is allowed to be crawled.
- **Utils**: A set of utility functions used by the other components.

## Usage

To run the crawler, you can execute the `searchlight-crawler` executable created in the `build/crawler` directory after building the project. The seed links are currently hardcoded in the `main.cpp` file.

```bash
./build/crawler/searchlight-crawler
```
