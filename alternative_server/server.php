<?php
// Enable error reporting
ini_set('display_errors', 1);
error_reporting(E_ALL);

// Set the content type to JSON, since the ESP32 is likely sending JSON data
header("Content-Type: application/json");

// Database connection settings
$servername = "localhost";
$username = "root";
$password = "";
$database = "capstone";

$conn = new mysqli($servername, $username, $password, $database);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} else {
    echo "Connected to the database successfully!\n";
}

// Read and parse the incoming POST request
$data = json_decode(file_get_contents("php://input"), true);

if (isset($data['liter']) && isset($data['percent'])) {
    $liter = $data['liter'];
    $percent = $data['percent'];

    // Prepare the SQL query to insert data into the database
    $stmt = $conn->prepare("INSERT INTO iv_data (liter, percent) VALUES (?, ?)");
    $stmt->bind_param("dd", $liter, $percent);

    if ($stmt->execute()) {
        echo json_encode(["message" => "Data received!"]);
    } else {
        http_response_code(500);
        echo json_encode(["error" => "Database error: " . $stmt->error]);
    }

    $stmt->close();
} else {
    http_response_code(400);
    echo json_encode(["error" => "Invalid data"]);
}

$conn->close();
?>
