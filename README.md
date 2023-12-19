# minilog

```sql
CREATE TABLE logs (
    id INT AUTO_INCREMENT,
    log_time DATETIME(3),
    level VARCHAR(50),
    message TEXT,
    filename VARCHAR(255),
    linenumber INT,
    PRIMARY KEY (id)
);
```

```sql
INSERT INTO logs (log_time, level, message, filename, linenumber) VALUES
('2023-03-06 10:00:00', 'info', 'Log message 1', 'file1.log', 10);
INSERT INTO logs (log_time, level, message, filename, linenumber) VALUES
('2023-03-06 10:00:00.111', 'error', 'Log message 1', 'file1.log', 10);
```
