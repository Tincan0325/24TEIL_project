import os

if __name__=='__main__':
    file_loc = os.path.join(os.getcwd(), 'babyslakh_16k')
    track = os.listdir(file_loc)
    for t in track:
        stems = os.path.join(file_loc, t, 'stems')
        s = os.listdir(stems)
        for f in s:
            if f.startswith('._'): 
                src = os.path.join(stems, f)
                f = f[2:]  
                des = os.path.join(stems, f)
                print(src, des)
                #os.rename(src, des)