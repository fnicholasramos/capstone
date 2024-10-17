const express = require('express');
const mysql = require('mysql');
const bodyParser = require('body-parser');

const app = express();
const port = 3000;

// MySQL connection
const db = mysql.createConnection({
  host: 'localhost',
  user: 'root',
  password: '',
  database: 'capstone'
});

// Connect to MySQL
db.connect((err) => {
  if (err) throw err;
  console.log('Connected to database successfully!');
});

// Middleware to parse JSON bodies
app.use(bodyParser.json());

// API endpoint to receive data from the ESP32
app.post('/data', (req, res) => {
  const { liter, percent } = req.body;
  
  if (typeof liter !== 'undefined' && typeof percent !== 'undefined') {
    // Insert into the database
    const query = 'INSERT INTO iv_data (liter, percent) VALUES (?, ?)';
    db.query(query, [liter, percent], (err, result) => {
      if (err) {
        res.status(500).send('Database error');
        throw err;
      }
      res.send('Data received');
    });
  } else {
    res.status(400).send('Invalid data');
  }
});

// Start the server
app.listen(port, '0.0.0.0', () => {
  console.log(`Server is running on port ${port}`);
});
