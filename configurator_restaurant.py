import tkinter
from tkinter import *
from tkinter import ttk
from tkinter import messagebox
from tkinter.simpledialog import askstring
import time
import serial                                                                                       # type: ignore

arduino = serial.Serial(port = 'COM8', baudrate = 115200, timeout=.1)

window = tkinter.Tk()

background_image = tkinter.PhotoImage(file="posibil1.png")
background_label = tkinter.Label(window, image=background_image)
background_label.place(x=0, y=0, relwidth=1, relheight=1)

window.resizable(False, False)  

window_height = 430
window_width = 600

screen_width = window.winfo_screenwidth()
screen_height = window.winfo_screenheight()

x_cordinate = int((screen_width/2) - (window_width/2))
y_cordinate = int((screen_height/2) - (window_height/2))

window.geometry("{}x{}+{}+{}".format(window_width, window_height, x_cordinate, y_cordinate))

window.title("Configurare restaurant")
main_frame = tkinter.Frame(window)
map_buttons = [[]]

keeper = list()

def send_info_to_arduino(n,m):
    tables_counter = 0
    sended_string = ''
    for i in range(n):
        for j in range(m):
            if keeper[i][j] < 0: # table case
                tables_counter += 1
                sended_string += 'M'
                if keeper[i][j] > -10:
                    sended_string += '0'
                sended_string += str(-1 * keeper[i][j])
            else:
                sended_string += (str)(keeper[i][j])

    ranges = ''
    if n < 9:
        ranges += '0'
    ranges += str(n)
    if m < 9:
        ranges += '0'
    ranges += str(m)
    if tables_counter < 9:
        ranges += '0'
    ranges += str(tables_counter)

    arduino.write(bytes(ranges, 'utf-8'))
    arduino.write(bytes(sended_string, 'utf-8'))

    time.sleep(5)
    

def map_button_pressed(i, j, radio_button_var):

    if radio_button_var == '0':
        map_buttons[i + 1][j].configure(text = "0", bg = 'white')
        keeper[i][j] = 0

    if radio_button_var == '1':
        map_buttons[i+1][j].configure(text = "D", bg = 'green3')
        keeper[i][j] = 1

    if radio_button_var == '2':
        map_buttons[i + 1][j].configure(text = "B" , bg = 'yellow1')
        keeper[i][j] = 2

    if radio_button_var == '3':
        table_number = askstring("?", "Precizati numarul mesei")
        if isinstance(table_number,str) and table_number.isnumeric() and table_number !="" and int(table_number) > 0:
            map_buttons[i + 1][j].configure(text = "M" + table_number, bg = 'cyan2')
            table_number = int(table_number)
            table_number = -1 * table_number
            keeper[i][j] = table_number


def confirm_pressed():
    input_length = length_data.get()
    input_width = width_data.get()
    if (not(input_length.isnumeric() and input_width.isnumeric())) or ( (int)(input_length) <=0 or (int)(input_width) <=0 or (int)(input_length) >20 or (int)(input_width) >20):
        warning_label.grid()
        warning_data.grid()
        return


    m = (int)(input_length)
    n = (int)(input_width)
    for i in range(n):
        aux = list()
        for j in range(m):
            aux.append(0)
        keeper.append(aux)

    window.destroy()

    map_frame = tkinter.Tk()
    map_frame.resizable(False, False)  # This code helps to disable windows from resizing
    
    background_image = tkinter.PhotoImage(file="restaurantplan_nobg.png")
    background_label = tkinter.Label(map_frame, image=background_image)
    background_label.place(x=0, y=0, relwidth=1, relheight=1)

    map_frame_height = 600
    map_frame_width = 980

    screen_width = map_frame.winfo_screenwidth()
    screen_height = map_frame.winfo_screenheight()

    x_cordinate = int((screen_width/2) - (map_frame_width/2))
    y_cordinate = int((screen_height/2) - (map_frame_height/2))

    if n>15:
        map_frame_height = 900
        map_frame_width = 1240
    map_frame.geometry("{}x{}+{}+{}".format(map_frame_width, map_frame_height, x_cordinate, y_cordinate))
    map_frame.title("Harta restaurantului")

    button_frame = tkinter.Frame(map_frame)
    button_frame.grid(row=1, column=1, columnspan=m, rowspan=n, pady=10, padx=10)
    
    var = tkinter.StringVar()  # used for the radio buttons
    var.set(None)
    for i in range(n):
        row_buttons = []
        for j in range(m):
            b = tkinter.Button(button_frame, text="0", command=lambda k=i, l=j: map_button_pressed(k, l, var.get()), width=5, height=2, font=("Arial", 12), bg="white", fg="black")
            b.grid(row=i, column=j, sticky="news")
            row_buttons.append(b)
        map_buttons.append(row_buttons)

    radio_button_text = tkinter.Label(map_frame, bg="lightgrey", text="Configurați spațiul folosind următoarele elemente:", font=("Arial", 13, "bold"))
    radio_button_text.grid(row=1, column=m + 2, columnspan=2, sticky="s")
    road_button = tkinter.Radiobutton(map_frame, bg="green3",justify="left", text= "Drum", variable=var,  value = 1)
    road_button.grid(row = 2 ,column = m + 2, sticky="w")
    kitchen_button = tkinter.Radiobutton(map_frame,bg="yellow1",justify="left", text= "Bucătărie", variable=var, value = 2)
    kitchen_button.grid(row = 3 ,column = m + 2, sticky="w")
    table_button = tkinter.Radiobutton(map_frame,bg="cyan2",justify="left",  text= "Masă", variable=var, value = 3)
    table_button.grid(row = 4 ,column = m + 2, sticky="w")
    clear_button_text = tkinter.Label(map_frame, bg="lightgrey", text="Selectați pentru ștergerea unui element", font=("Arial", 13, "bold"))
    clear_button_text.grid(row=7, column=m + 2, columnspan=2, sticky="sw")
    clear_button = tkinter.Radiobutton(map_frame, bg="orangered", justify="left",text= "Șterge", variable=var, value = 0)
    clear_button.grid(row = 8 ,column = m + 2, sticky="w")

    done_button = tkinter.Button(map_frame, bg="green", fg="white", text= "Finalizare", command = lambda: send_info_to_arduino(n,m),font=("Arial", 13))
    done_button.grid(row = n + 1, column=m//2, columnspan = 2)
    close_button = tkinter.Button(map_frame, bg="red", fg="white", text="Ieșire", command = lambda: map_frame.destroy(),font=("Arial", 13))
    close_button.grid(row=n + 1,column=m+3, columnspan=2)


    map_frame.mainloop()


form_frame = tkinter.Frame(main_frame, bg="lightgrey", padx=20, pady=20, borderwidth=2, relief="groove")
form_frame.grid(row=3, column=0, columnspan=2)
form_frame.lift()

form_title = tkinter.Label(form_frame, text="Configurează spațiul restaurantului", font=("Arial", 14, "bold"), bg="lightgray")
form_title.grid(row=0, column=0, columnspan=2, pady=10)

length_label = tkinter.Label(form_frame, text="Lungime restaurant (în metri)", font=("Arial", 12), bg="lightgrey")
length_label.grid(row=1, column=0, padx=20, pady=(10, 0) )

length_data = tkinter.Entry(form_frame, width=20, font=("Arial", 12))
length_data.grid(row=2, column=0, padx=10, pady=(0, 10))

width_label = tkinter.Label(form_frame, text="Lățime restaurant (în metri)", font=("Arial", 12), bg="lightgrey")
width_label.grid(row=3, column=0, padx=20, pady=(10, 0))

width_data = tkinter.Entry(form_frame, width=20, font=("Arial", 12))
width_data.grid(row=4, column=0, padx=10, pady=(0, 10))

warning_label = tkinter.Label(form_frame, text="WARNING!", fg="red", font=("Arial", 12, "bold"))
warning_label.grid(row=3, column=0, columnspan=2, padx=0, pady=0)
warning_data = tkinter.Label(form_frame, text="Lațimea și lungimea trebuie să fie numere intregi pozitive", fg="red", font=("Arial", 12))
warning_data.grid(row=4, column=0, columnspan=2, padx=0, pady=0)

confirm_button = tkinter.Button(form_frame, text="Confirma", command=confirm_pressed, bg="green", fg="white", font=("Arial", 12))
confirm_button.grid(row=5, column=0, columnspan=2, pady=50)

warning_label.grid_remove()
warning_data.grid_remove()

main_frame.pack()
window.mainloop()
