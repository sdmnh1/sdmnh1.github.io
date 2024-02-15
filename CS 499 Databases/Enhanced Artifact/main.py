# CS 499 Computer Science Capstone - Enchancement Three - Databases
# =============================================================================
#  Date: February 10, 2024
#  Created By : Maria Herring   Email: maria.herring@shnu.edu
#  File Name : Enhancement Three
#  This project is based off of coursework completed in 
#  SNHU DAD 220 - Introduction into Structure Databases
#
#  Description : This main.py file contains the functionality which creates the user
#  interface for the fleet management system. The user interface is created using tkinter.
#  The user interface allows the user to add, delete, and update records in the database.
#  The user interface also allows the user to view the records in the database.
# =============================================================================

# Imports
import tkinter as tk
from tkinter import *
from tkinter import ttk, messagebox
from user import User
from tkinter import Scrollbar

backgroundColor = 'aliceblue'
root = tk.Tk()
root.title("Fleet Management System")

width = 1200
height = 600
screen_width = root.winfo_screenwidth()
screen_height = root.winfo_screenheight()
x = (screen_width - width) // 2
y = (screen_height - height) // 2
root.geometry(f"{width}x{height}+{x}+{y}")

# Disable window resizing
root.resizable(False, False)

def home():
    global MainFrame
    MainFrame = Frame(root)
    MainFrame.place(x=0, y=0)
    MainFrame.pack(side=TOP, pady=10)

    # Main Heading
    main_heading = Label(MainFrame, text="Records", font=('Calibri', 25, "bold"))
    main_heading.grid(row=0, column=0, columnspan=5, padx=(20, 20), sticky="n")
    # Treeview for the records
    trv = ttk.Treeview(MainFrame, selectmode='browse', height=20)
    trv.grid(row=2, column=0, columnspan=5, padx=20, pady=20)
    # Scrollbar for the treeview
    vsb = Scrollbar(MainFrame, orient="vertical", command=trv.yview)
    vsb.grid(row=2, column=5, pady=5, sticky='ns')
    trv.configure(yscrollcommand=vsb.set)

    # Column identifiers for the treeview
    trv["columns"] = ("1", "2", '3', '4', '5', '6', '7', '8')
    trv['show'] = 'headings'
    # Width and alignment of the columns
    trv.column("1", width=50, anchor='c')
    trv.column("2", width=100, anchor='c')
    trv.column("3", width=100, anchor='c')
    trv.column("4", width=180, anchor='c')
    trv.column("5", width=180, anchor='c')
    trv.column("6", width=100, anchor='c')
    trv.column("7", width=180, anchor='c')
    trv.column("8", width=250, anchor='c')
    # Heading labels for the columns
    trv.heading("1", text="ID")
    trv.heading("2", text="Vehicle ID")
    trv.heading("3", text="State")
    trv.heading("4", text="Repair")
    trv.heading("5", text="Reason")
    trv.heading("6", text="YEAR")
    trv.heading("7", text="Make")
    trv.heading("8", text="Body Type")

    def selectItem(a):
        global curItem
        curItem = trv.focus()
        cur = trv.item(curItem)
        # print(trv.item(curItem))
        global selected
        selected = trv.item(curItem)
        # selected = trv.item(curItem)['values'][0]
        # print(selected)

    trv.bind('<ButtonRelease-1>', selectItem)

    user = User()
    user.csv_to_database()
    data = user.get_all_data()

    for data in data:
        trv.insert("", 'end', values=(data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]))

    # Delete function with error code for not selecting an item 
    def delete_function():
        try:
            # Retrieve the id of the selected item
            if 'selected' not in globals() or not selected['values']:
                messagebox.showerror("Error", "Please select the record item for deletion!")
                return
            
            id = selected['values'][0]
            
            # Check for an empty id
            if str(id).strip() == "":
                messagebox.showerror("Error", "Please select the record item for deletion!")
                return  
            
            # Delete record
            user = User()
            user.delete_by_id(id)
            
            messagebox.showinfo("Success", "Record deleted successfully.")          
            # Clear the values, return to home screen
            selected['values'] = [] 
            MainFrame.destroy() 
            home()
            
        except Exception as e:
            # Exception error message
            messagebox.showerror("Error", "An error occurred while deleting the record: " + str(e))
            
    # Create the exit, create, update, and delete buttons
    exit_button = ttk.Button(MainFrame, style="Rounded.TButton", text="Exit", command=Exit, cursor='hand2')
    exit_button.grid(row=4, column=0, padx=(5, 20), sticky="e")

    create_button = ttk.Button(MainFrame, style="Rounded.TButton", text="Create", command=toggleToCreate,
                               cursor='hand2')
    create_button.grid(row=4, column=1, padx=(5, 20), sticky="e")

    update_button = ttk.Button(MainFrame, style="Rounded.TButton", text="Update", command=toggleToUpdate,
                               cursor='hand2')
    update_button.grid(row=4, column=2, padx=(5, 20), sticky="e")

    delete_button = ttk.Button(MainFrame, style="Rounded.TButton", text="Delete", command=delete_function,
                               cursor='hand2')
    delete_button.grid(row=4, column=3, padx=(5, 20), sticky="e")

# Exit the application
def Exit():
    root.destroy()

# Enter create window
def toggleToCreate():
    MainFrame.destroy();
    create()
    
# Enter update window, error message if no item is selected
def toggleToUpdate():
    if 'selected' not in globals() or not selected['values']:
        messagebox.showerror("Error", "please select item for to update.")
    else:
        MainFrame.destroy()
        update()

# Method for update window along with the frames and labels for all record attributes
def update():
    global updateFrame, lbl_login_error
    updateFrame = Frame(root)
    updateFrame.place(x=0, y=0)
    updateFrame.pack(side=TOP, pady=10)

    lbl = Label(updateFrame, text="Update the Record", fg='black', font=('Calibri', 25, "bold"))
    lbl.grid(row=0, pady=50, columnspan=2, padx=60)

    id_label = Label(updateFrame, text="Vehicle Id:", font=('Calibri', 15))
    id_label.grid(row=2, column=0, padx=20, sticky="e")
    id_entry = Entry(updateFrame, font=('Calibri', 15), width=30)
    id_entry.grid(row=2, column=1, pady=10, sticky="w")
    id_entry.insert(0, selected['values'][1])

    state_label = Label(updateFrame, text="State:", font=('Calibri', 15))
    state_label.grid(row=3, column=0, padx=20, sticky="e")
    state_entry = Entry(updateFrame, font=('Calibri', 15), width=30)
    state_entry.grid(row=3, column=1, pady=10, sticky="w")
    state_entry.insert(0, selected['values'][2])

    repair_label = Label(updateFrame, text="Repair:", font=('Calibri', 15))
    repair_label.grid(row=4, column=0, padx=20, sticky="e")
    repair_entry = Entry(updateFrame, font=('Calibri', 15), width=30)
    repair_entry.grid(row=4, column=1, pady=10, sticky="w")
    repair_entry.insert(0, selected['values'][3])

    reason_label = Label(updateFrame, text="Reason:", font=('Calibri', 15))
    reason_label.grid(row=5, column=0, padx=20, sticky="e")
    reason_entry = Entry(updateFrame, font=('Calibri', 15), width=30)
    reason_entry.grid(row=5, column=1, pady=10, sticky="w")
    reason_entry.insert(0, selected['values'][4])

    year_label = Label(updateFrame, text="Year:", font=('Calibri', 15))
    year_label.grid(row=6, column=0, padx=20, sticky="e")
    year_entry = Entry(updateFrame, font=('Calibri', 15), width=30)
    year_entry.grid(row=6, column=1, pady=10, sticky="w")
    year_entry.insert(0, selected['values'][5])

    make_label = Label(updateFrame, text="Make:", font=('Calibri', 15))
    make_label.grid(row=7, column=0, padx=20, sticky="e")
    make_entry = Entry(updateFrame, font=('Calibri', 15), width=30)
    make_entry.grid(row=7, column=1, pady=10, sticky="w")
    make_entry.insert(0, selected['values'][6])

    body_type_label = Label(updateFrame, text="Body Type:", font=('Calibri', 15))
    body_type_label.grid(row=8, column=0, padx=20, sticky="e")
    body_type_entry = Entry(updateFrame, font=('Calibri', 15), width=30)
    body_type_entry.grid(row=8, column=1, pady=10, sticky="w")
    body_type_entry.insert(0, selected['values'][7])

    # Update function to edit attributes, with error code for all required fields
    def update_function():
        try:       
            if 'selected' not in globals() or not selected['values']:
                messagebox.showerror("Error", "Please select the record item to update!")  
                return
           
            id = selected['values'][0]
            veh_id = id_entry.get()
            state = state_entry.get()
            repair = repair_entry.get()
            reason = reason_entry.get()
            year = year_entry.get()
            make = make_entry.get()
            body = body_type_entry.get()
            print(veh_id)
            
            if any(not str(val).strip() for val in (veh_id, state, repair, reason, year, make, body)):  
                messagebox.showerror("Error", "Please fill in all fields.") 
                return
            
            user = User()
            user.update_by_id(id, veh_id, state, repair, reason, year, make, body)  
            
            messagebox.showinfo("Success", "Record update successful.")       
            selected['values'] = [] 
            updateFrame.destroy()   
            home()
            
        except Exception as e:
            messagebox.showerror("Error", "An error occurred while updating the record: " + str(e))
            return
                 
    # Update button
    login_button = ttk.Button(updateFrame, style="Rounded.TButton", text="Update", command=update_function,
                              cursor="hand2")
    login_button.grid(row=10, columnspan=2, pady=20, padx=120, sticky="w")
    
    # Back button
    back_button = ttk.Button(updateFrame, style="Rounded.TButton", text="Back", command=toggleuBack, cursor="hand2")
    back_button.grid(row=10, column=1, pady=(20, 20), padx=(120, 60), sticky="w")

# Back to home page
def toggleuBack():
    updateFrame.destroy();
    home()

# Method for create window along with the frames and labels for all record attributes
def create():
    global createFrame, lbl_login_error
    createFrame = Frame(root)
    createFrame.place(x=0, y=0)
    createFrame.pack(side=TOP, pady=10)

    lbl = Label(createFrame, text="Add New Record", fg='black', font=('Calibri', 25, "bold"))
    lbl.grid(row=0, pady=50, columnspan=2, padx=60)

    id_label = Label(createFrame, text="Vehicle Id:", font=('Calibri', 15))
    id_label.grid(row=2, column=0, padx=20, sticky="e")
    id_entry = Entry(createFrame, font=('Calibri', 15), width=30)
    id_entry.grid(row=2, column=1, pady=10, sticky="w")

    state_label = Label(createFrame, text="State:", font=('Calibri', 15))
    state_label.grid(row=3, column=0, padx=20, sticky="e")
    state_entry = Entry(createFrame, font=('Calibri', 15), width=30)
    state_entry.grid(row=3, column=1, pady=10, sticky="w")

    repair_label = Label(createFrame, text="Repair:", font=('Calibri', 15))
    repair_label.grid(row=4, column=0, padx=20, sticky="e")
    repair_entry = Entry(createFrame, font=('Calibri', 15), width=30)
    repair_entry.grid(row=4, column=1, pady=10, sticky="w")

    reason_label = Label(createFrame, text="Reason:", font=('Calibri', 15))
    reason_label.grid(row=5, column=0, padx=20, sticky="e")
    reason_entry = Entry(createFrame, font=('Calibri', 15), width=30)
    reason_entry.grid(row=5, column=1, pady=10, sticky="w")

    year_label = Label(createFrame, text="Year:", font=('Calibri', 15))
    year_label.grid(row=6, column=0, padx=20, sticky="e")
    year_entry = Entry(createFrame, font=('Calibri', 15), width=30)
    year_entry.grid(row=6, column=1, pady=10, sticky="w")

    make_label = Label(createFrame, text="Make:", font=('Calibri', 15))
    make_label.grid(row=7, column=0, padx=20, sticky="e")
    make_entry = Entry(createFrame, font=('Calibri', 15), width=30)
    make_entry.grid(row=7, column=1, pady=10, sticky="w")

    body_type_label = Label(createFrame, text="Body Type:", font=('Calibri', 15))
    body_type_label.grid(row=8, column=0, padx=20, sticky="e")
    body_type_entry = Entry(createFrame, font=('Calibri', 15), width=30)
    body_type_entry.grid(row=8, column=1, pady=10, sticky="w")

    lbl_login_error = Label(createFrame, text="", font=('Calibri', 12), cursor="hand2", foreground='red')
    lbl_login_error.grid(row=10, column=1, pady=10)
    
    # Create function to create a new record, with error code for all required fields
    def create_function():
        try:
            id = id_entry.get()
            state = state_entry.get()
            repair = repair_entry.get()
            reason = reason_entry.get()
            year = year_entry.get()
            make = make_entry.get()
            body = body_type_entry.get()
            
            if any(not str(val).strip() for val in (id, state, repair, reason, year, make, body)):  
                messagebox.showerror("Error", "Please fill in all fields.")     
                return
            
            user = User()
            user.create_record(id, state, repair, reason, year, make, body) 
            
            messagebox.showinfo("Success", "Record created successfully.")      
            createFrame.destroy()   
            home()
            
        except Exception as e:  
            messagebox.showerror("Error", "An error occurred creating this record. Please try again: " + str(e))    
            return
     
    # Create button
    login_button = ttk.Button(createFrame, style="Rounded.TButton", text="Create", command=create_function,
                              cursor="hand2")
    login_button.grid(row=10, columnspan=2, pady=20, padx=120, sticky="w")
   
    # Back button
    back_button = ttk.Button(createFrame, style="Rounded.TButton", text="Back", command=togglecBack, cursor="hand2")
    back_button.grid(row=10, column=1, pady=(20, 20), padx=(120, 60), sticky="w")

# Back to home page
def togglecBack():
    createFrame.destroy();
    home()

style = ttk.Style()
style.configure("Rounded.TButton", borderwidth=5, relief="solid", padding=(10, 10))
home()
root.mainloop()
