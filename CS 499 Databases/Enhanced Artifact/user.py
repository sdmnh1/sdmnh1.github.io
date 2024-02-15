# CS 499 Computer Science Capstone - Enchancement Three - Databases
# =============================================================================
#  Date: February 10, 2024
#  Created By : Maria Herring   Email: maria.herring@shnu.edu
#  File Name : Enhancement Three
#  This project is based off of coursework completed in 
#  SNHU DAD 220 - Introduction into Structure Databases
#
#  Description : This user.py file contains the functionality which initalizes the 
#  database connection and checks for table data The user class is used to interact 
#  with the database once the CSV file data is imported. The user class also contains 
#  methods containing SQL queries to read, create, delete, and update records in the database.
# =============================================================================

# Imports
import csv
import pymysql

class User:
    # Initialize the class
    def __init__(self):
        self.DB_HOST = 'localhost'
        self.DB_USERNAME = 'user'
        self.DB_PASSWORD = 'welcome123'
        self.DB_NAME = 'fleet'
        self.TABLE_NAME = 'Records'
        self.csv_file_path = 'Fleet.csv'
        self.connection = self.connect_db()
    
    # Method to establish the database connection
    def connect_db(self):
        connection = pymysql.connect(
            host=self.DB_HOST,
            user=self.DB_USERNAME,
            password=self.DB_PASSWORD,
            database=self.DB_NAME
        )
        return connection
 
    # Method to check if the table is empty
    def is_table_empty(self):
        cursor = self.connection.cursor()
        query = f"SELECT COUNT(*) FROM {self.TABLE_NAME}"
        cursor.execute(query)
        count = cursor.fetchone()[0]
        cursor.close()
        return count == 0

    # Method to import CSV file data
    def csv_to_database(self):
        if not self.is_table_empty():
            print("skipping CSV import.")
            return

        cursor = self.connection.cursor()
        csv_file_path = self.csv_file_path
        with open(csv_file_path, 'r') as file:
            csv_data = csv.reader(file)
            next(csv_data)

            for row in csv_data:
                query = "INSERT INTO {} (`Vehicle ID`, `State`, `Repair`, `Reason`, `YEAR`, `Make`, `Body Type`) VALUES (%s, %s, %s, %s, %s, %s, %s)".format(self.TABLE_NAME)   
                cursor.execute(query, row)

        self.connection.commit()
        cursor.close()
        print("CSV imported.")
        
    # Method to get all data
    def get_all_data(self):
        cursor = self.connection.cursor()
        query = f"SELECT * FROM {self.TABLE_NAME}"
        cursor.execute(query)
        rows = cursor.fetchall()
        cursor.close()

        return rows
        
    # Method to create a new record
    def create_record(self, vehicle_id, state, repair, reason, year, make, body_type):
        cursor = self.connection.cursor()
        query = f"INSERT INTO {self.TABLE_NAME} (`Vehicle ID`, `State`, `Repair`, `Reason`, `YEAR`, `Make`, `Body Type`) VALUES (%s, %s, %s, %s, %s, %s, %s)"
        data = (vehicle_id, state, repair, reason, year, make, body_type)
        cursor.execute(query, data)
        self.connection.commit()
        cursor.close()

    # Method to delete a record by ID
    def delete_by_id(self, id):
        cursor = self.connection.cursor()
        # Deleting records based on Vehicle ID
        query = f"DELETE FROM {self.TABLE_NAME} WHERE `ID` = %s"
        cursor.execute(query, (id,))

        self.connection.commit()
        cursor.close()

    # Method to update a record by ID 
    def update_by_id(self,id, vehicle_id, state, repair, reason, year, make, body_type):
        print(id, vehicle_id, state, repair,year,make, body_type)
        cursor = self.connection.cursor()
        query = f"UPDATE {self.TABLE_NAME} SET `Vehicle ID`= %s, `State` = %s, `Repair` = %s, `Reason` = %s, `YEAR` = %s, `Make` = %s, `Body Type` = %s WHERE `ID` = %s"
        data = (vehicle_id, state, repair, reason, year, make, body_type, id)
        cursor.execute(query, data)

        self.connection.commit()
        cursor.close()