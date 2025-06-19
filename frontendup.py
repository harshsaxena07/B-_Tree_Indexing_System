import tkinter as tk
from tkinter import messagebox
from tkinter import simpledialog
import subprocess

class BPlusTreeApp:
    def __init__(self, root):
        self.root = root
        self.root.title("TreeMendous_Four")

        # Set background color to black
        self.root.config(bg='black')

        self.root.rowconfigure(0, weight=0)  # No top padding
        self.root.rowconfigure(2, weight=1)  # Keep bottom padding

        self.root.columnconfigure(0, weight=1)  # left space
        self.root.columnconfigure(1, weight=0)  # content
        self.root.columnconfigure(2, weight=1)  # right space


        self.root.bind("<Escape>", lambda event: self.root.destroy())
        # Define the labels and entry fields for various operations
        self.create_widgets()

    def create_widgets(self):
        # Create a centered frame
        content_frame = tk.Frame(self.root, bg='black')
        content_frame.grid(row=1, column=1)  # place in center column

        # Set larger font size and white text color
        heading_frame = tk.LabelFrame(
            content_frame,
            bg='black',
            bd=2,
            relief='solid',
            highlightbackground='#ffffff',   # dark grey border
            highlightcolor='#ffffff',
            highlightthickness=2
        )
        heading_frame.grid(row=0, column=0, columnspan=2, pady=(10, 20), padx=10, sticky="ew")


        tk.Label(
            heading_frame,
            text="DATA FETCHER USING B+ TREE",
            font=('Arial', 26, 'bold'),
            fg='#FFD700',
            bg='black'
        ).pack(padx=10, pady=10)

        font = ('Arial', 12)
        fg_color = 'white'

        tk.Label(content_frame, text="Data File:", font=('Arial', 13, 'bold'), fg=fg_color, bg='black').grid(row=1, column=0, pady=10, padx=10, sticky="w")
        self.data_file_entry = tk.Entry(content_frame, font=font, width=40, fg='black')
        self.data_file_entry.insert(0, "Enter your data file name")
        self.data_file_entry.bind("<FocusIn>", lambda event: self.clear_placeholder(event, "Enter your data file name"))
        self.data_file_entry.bind("<FocusOut>", lambda event: self.add_placeholder(event, "Enter your data file name"))
        self.data_file_entry.grid(row=1, column=1, pady=10, padx=10)

        tk.Label(content_frame, text="Index File:", font=('Arial', 13, 'bold'), fg=fg_color, bg='black').grid(row=2, column=0, pady=10, padx=10, sticky="w")
        self.index_file_entry = tk.Entry(content_frame, font=font, width=40, fg='black')
        self.index_file_entry.insert(0, "Enter your index file name")
        self.index_file_entry.bind("<FocusIn>", lambda event: self.clear_placeholder(event, "Enter your index file name"))
        self.index_file_entry.bind("<FocusOut>", lambda event: self.add_placeholder(event, "Enter your index file name"))
        self.index_file_entry.grid(row=2, column=1, pady=10, padx=10)

        tk.Label(content_frame, text="Key Length:", font=('Arial', 13, 'bold'), fg=fg_color, bg='black').grid(row=3, column=0, pady=10, padx=10, sticky="w")
        self.key_length_entry = tk.Entry(content_frame, font=font, width=40, fg='black')
        self.key_length_entry.insert(0, "Enter key length")
        self.key_length_entry.bind("<FocusIn>", lambda event: self.clear_placeholder(event, "Enter key length"))
        self.key_length_entry.bind("<FocusOut>", lambda event: self.add_placeholder(event, "Enter key length"))
        self.key_length_entry.grid(row=3, column=1, pady=10, padx=10)

        button_font = ('Arial', 12)
        # Common button styling
        button_style = {
            "font": ('Arial', 12, 'bold'),
            "bg": '#9370DB',               # Medium Purple
            "fg": 'white',
            "activebackground": '#7B68EE', # Slate Blue
            "relief": 'raised',
            "bd": 2,                       # reduced border thickness
            "padx": 6,                     # smaller horizontal padding
            "pady": 4,                     # smaller vertical padding
            "width": 18                   # reduced button width
        }

        tk.Label(
            content_frame,
            text="Features: Create Index, Insert Record, Find Record, and List Records using B+ Tree.",
            font=('Arial', 12, 'italic'),
            fg='lightgray',
            bg='black'
        ).grid(row=4, column=0, columnspan=2, pady=(20, 10))
        
        # Button frame below feature label
        button_row_index = 6
        button_frame = tk.Frame(content_frame, bg='black')
        button_frame.grid(row=button_row_index, column=0, columnspan=2, pady=(10, 20))
        button_frame.columnconfigure(0, weight=1)
        button_frame.columnconfigure(1, weight=1)

        tk.Button(button_frame, text="Create Index", command=self.create_index, **button_style).grid(
            row=0, column=0, padx=25, pady=15, sticky="ew"
        )
        tk.Button(button_frame, text="Insert Record", command=self.insert_record,  **{**button_style, "bd": 3}).grid(
            row=0, column=1, padx=25, pady=15, sticky="ew"
        )
        tk.Button(button_frame, text="Find Record", command=self.find_record, **button_style).grid(
            row=1, column=0, padx=25, pady=15, sticky="ew"
        )
        tk.Button(button_frame, text="List Records", command=self.list_records,  **{**button_style, "bd": 3}).grid(
            row=1, column=1, padx=25, pady=15, sticky="ew"
        )


        # Output Label
        tk.Label(
            content_frame,
            text="Output Result",
            bg="black",
            fg="#FFD700",  # Golden yellow
            font=("Arial", 16, "bold")
        ).grid(row=8, column=0, columnspan=2, padx=10, pady=(30, 10), sticky="w")

        # Output Text Area (larger, styled, with placeholder)
        self.result_text = tk.Text(
            content_frame,
            width=70,              # Increased width
            height=10,             # Increased height
            font=('Courier New', 14),
            wrap=tk.WORD,
            bg="#1e1e1e",          # Dark background
            fg="white",            # Text color
            bd=3,                  # Border thickness
            relief="groove",       
            highlightthickness=1,
            highlightbackground="#FFD700"  # Golden border
        )
        self.result_text.grid(row=9, column=0, columnspan=2, pady=(5, 30), padx=10)

        # Insert placeholder
        self.result_text.insert("1.0", "Output will be displayed here...")
        self.result_text.config(fg="#888888")  # Light gray placeholder color
        self.result_text.config(state="disabled")

        # Remove placeholder on focus
        def clear_placeholder(event):
            if self.result_text.get("1.0", "end-1c") == "Output will be displayed here...":
                self.result_text.delete("1.0", tk.END)
                self.result_text.config(fg="black")

        # Restore placeholder if empty
        def restore_placeholder(event):
            if self.result_text.get("1.0", "end-1c") == "":
                self.result_text.insert("1.0", "Output will be displayed here...")
                self.result_text.config(fg="#888888")

        self.result_text.bind("<FocusIn>", clear_placeholder)
        self.result_text.bind("<FocusOut>", restore_placeholder)

        # Close Button below output result
        tk.Button(
            content_frame,
            text="Close Application",
            command=self.root.destroy,
            font=('Arial', 12, 'bold'),
            bg='red',
            fg='white',
            activebackground='#B22222',  # firebrick red
            relief="raised",
            bd=3,
            padx=10,
            pady=5,
            width=20
        ).grid(row=10, column=0, columnspan=2, pady=(0, 30))



        self.center_window()


    def center_window(self):
        # Get screen width and height
        screen_width = self.root.winfo_screenwidth()
        screen_height = self.root.winfo_screenheight()

        # Define window content size (adjust as needed)
        window_width = 600  # Define the width for the content
        window_height = 500  # Define the height for the content

        # Calculate position to center the content
        position_top = int(screen_height / 2 - window_height / 2)
        position_left = int(screen_width / 2 - window_width / 2)

        # Set window size and position to keep it centered
        self.root.geometry(f'{window_width}x{window_height}+{position_left}+{position_top}')

        # Make window fullscreen but centered
        self.root.attributes('-fullscreen', True)
    
    def create_index(self):
        data_file = self.data_file_entry.get()
        index_file = self.index_file_entry.get()
        key_length = int(self.key_length_entry.get())
        
        # Run the command to create the index
        command = f"btree.exe -create {data_file} {index_file} {key_length}"
        output = self.run_command(command)
        
        # Show result in text box
        if output:
            self.result_text.config(state="normal", fg = "white")
            self.result_text.delete(1.0, tk.END)
            self.result_text.insert(tk.END, output)
            self.result_text.config(state="disabled")

    def insert_record(self):
        index_file = self.index_file_entry.get()
        record = simpledialog.askstring("Input", "Enter record to insert (e.g., 230111565A ABHISHEK_KUMAR B-TECH DATA_SCIENCE I2 27):")
        if record:
            # Run the command to insert the record
            command = f'btree.exe -insert {index_file} "{record}"'
            output = self.run_command(command)
            
            # Show result in text box
            if output:
                self.result_text.config(state="normal", fg = "white")
                self.result_text.delete(1.0, tk.END)
                self.result_text.insert(tk.END, output)
                self.result_text.config(state="disabled")

    def find_record(self):
        index_file = self.index_file_entry.get()
        key = simpledialog.askstring("Input", "Enter key to find (e.g., 230111482A):")
        if key:
            key = key.strip()  # Remove extra spaces
            print(f"Searching for key: {key}")  # Debugging output
            command = f"btree.exe -find {index_file} {key}"
            output = self.run_command(command)
            
            # Show result in text box
            if not output:
                self.result_text.config(state="normal", fg = "white")
                self.result_text.delete(1.0, tk.END)
                self.result_text.insert(tk.END, "Record not found.")
                self.result_text.config(state="disabled")

            else:
                self.result_text.config(state="normal", fg = "white")
                self.result_text.delete(1.0, tk.END)
                self.result_text.insert(tk.END, output)
                self.result_text.config(state="disabled")
                
    def list_records(self):
        index_file = self.index_file_entry.get()
        if not index_file:
            messagebox.showerror("Input Error", "Please enter the index file name.")
            return
        index_file = index_file.strip()

        key = simpledialog.askstring("Input", "Enter starting key (e.g., 230111481A):")
        if not key:
            messagebox.showerror("Input Error", "Please enter a valid key.")
            return
        key = key.strip()

        count = simpledialog.askinteger("Input", "Enter number of records to list:")
        if not count or count <= 0:
            messagebox.showerror("Input Error", "Please enter a valid positive number for count.")
            return

        command = f"btree.exe -list {index_file} {key} {count}"
        print(f"Running command: {command}")  # Debug line

        output = self.run_command(command)

        # Show result in text box
        self.result_text.config(state="normal", fg = "white")
        self.result_text.delete(1.0, tk.END)
        if output:
            self.result_text.insert(tk.END, output)
        else:
            self.result_text.insert(tk.END, "No records found or error occurred.")
        self.result_text.config(state="disabled")

    def clear_placeholder(self, event, placeholder):
        if event.widget.get() == placeholder:
            event.widget.delete(0, tk.END)
            event.widget.config(fg='black')

    def add_placeholder(self, event, placeholder):
        if not event.widget.get():
            event.widget.insert(0, placeholder)
            event.widget.config(fg='black')

    def run_command(self, command):
        try:
            # Run the C++ executable as a subprocess and capture output
            output = subprocess.check_output(command, stderr=subprocess.STDOUT, shell=True, text=True)
            print(f"Command output: {output}")  # Debugging output
            return output
        except subprocess.CalledProcessError as e:
            print(f"Command error: {e.output}")  # Debugging output
            messagebox.showerror("Error", f"Error running command: {e.output}")
            return ""
        except FileNotFoundError:
            messagebox.showerror("Error", "Executable not found. Please ensure btree.exe is in the correct path.")
            return ""

# Run the Tkinter UI
if __name__ == "__main__":
    root = tk.Tk()
    app = BPlusTreeApp(root)
    root.mainloop()
