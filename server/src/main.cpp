#include "cpp-httplib/httplib.h"
#include <nlohmann/json.hpp>
#include "inja/inja.hpp"

// These are needed to convert the CMake macro to a C++ string
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

// Use nlohmann::json for convenience
using json = nlohmann::json;

int main(void) {
    // Create the server instance (like ServeMux)
    httplib::Server svr;
    
    // Create an Inja environment to handle templates
    inja::Environment env;

    // Define handlers (like http.HandleFunc)

    // Get the path to static files from the CMake definition
    const std::string static_path = TOSTRING(STATIC_FILE_PATH);
    
    svr.Get("/", [&](const httplib::Request &, httplib::Response &res) {
        // Use the static_path to build the full file path
        std::ifstream ifs(static_path + "/index.html");
        std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        res.set_content(content, "text/html");
    });
    
    svr.Get("/search", [&](const httplib::Request &req, httplib::Response &res) {
        // Get the query param "q"
        std::string query = req.has_param("q") ? req.get_param_value("q") : "";

        // --- SQLite FTS5 logic would go here ---
        json data;
        data["query"] = query;
        data["results"] = {"First result for '" + query + "'", "Second result!", "Third..."};
        
        // Render the template
        std::string result = env.render_file(static_path + "/results.html", data);
        
        res.set_content(result, "text/html");
    });

    std::cout << "Server listening on http://localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);
}
