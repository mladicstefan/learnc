
def main():
    x:list[int] = [1,2,3,4]
    
    print(f"x: {x}]") 
    print(f"id(x): {hex(id(x))}")            # Memory Adress of X object
    print(f"id(x[0]): {hex(id(x[0]))}")      # Address of first element object
    print(f"type(x): {type(x)}")        # <class 'list'>

    print(f"Last element {x[3]}")
    try:
        print(f"Trying to access out of bound element: {x[232]}")
    except Exception as e:
        raise e

if __name__ == "__main__":
    main()
