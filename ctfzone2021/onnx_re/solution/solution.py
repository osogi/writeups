import numpy as np
from onnx import load
from onnx.onnx_ONNX_REL_1_7_ml_pb2 import ModelProto
from onnxruntime.backend import prepare
import z3

def run_model(data: bytearray, model: ModelProto):
    """Run model and check result"""

    x = np.array(data, dtype=np.int64)
    i = np.array([0], dtype=np.int64)
    keep_going = np.array([True], dtype=np.bool)
    max_index = np.array([32], dtype=np.int64)
    model_rep = prepare(model)
    out = model_rep.run([x, i, keep_going, max_index])[1].reshape((1, 30))[0]
    assert np.array_equal(
        out,
        np.array(
            [
                16780,
                9831,
                2538,
                14031,
                8110,
                19136,
                17547,
                13929,
                16911,
                20265,
                15014,
                24203,
                9238,
                12759,
                17883,
                5871,
                15265,
                13222,
                11014,
                8290,
                7843,
                16989,
                7222,
                20835,
                15431,
                4554,
                8498,
                11543,
                6214,
                7169,
            ],
            dtype=np.int64,
        ),
    )
    print(f"Flag: ctfzone\x7b{data.decode()}\x7d")


def run_model_test(model: ModelProto):
    data=bytearray(("1"*32).encode())
    x = np.array(data, dtype=np.int64)
    i = np.array([0], dtype=np.int64)
    keep_going = np.array([True], dtype=np.bool)
    max_index = np.array([32], dtype=np.int64)
    model_rep = prepare(model)
    standard = model_rep.run([x, i, keep_going, max_index])[1].reshape((1, 30))[0]
    res=[]
    for c in range(0, 32):
        inp="1"*c+"2"+"1"*(31-c)
        data=bytearray((inp).encode())
        x = np.array(data, dtype=np.int64)
        i = np.array([0], dtype=np.int64)
        keep_going = np.array([True], dtype=np.bool)
        max_index = np.array([32], dtype=np.int64)
        model_rep = prepare(model)
        out = model_rep.run([x, i, keep_going, max_index])[1].reshape((1, 30))[0]
        buf=(out-standard)
        res.append(buf)
        print(str(c)+": "+str(buf))
    fin=[
                16780,
                9831,
                2538,
                14031,
                8110,
                19136,
                17547,
                13929,
                16911,
                20265,
                15014,
                24203,
                9238,
                12759,
                17883,
                5871,
                15265,
                13222,
                11014,
                8290,
                7843,
                16989,
                7222,
                20835,
                15431,
                4554,
                8498,
                11543,
                6214,
                7169,
            ]
    X = [ z3.Int('x%s' % i) for i in range(32) ]
    s = z3.Solver()
    for i in range(len(res[0])):
        string=""
        for b in range(len(res)):
            if(res[b][i]!=0):
                string+=f"{res[b][i]}*X[{b}]+"
        string=string[:-1]+f"=={fin[i]}"
        s.add(eval(string))
    for i in range(32):
        s.add(X[i]>=32, X[i]<126)
    print(s.check())
    for i in range(32):
        print(chr(int(s.model()[X[i]].as_string())), end="")
    print()

if __name__ == "__main__":
    model = load("model.onnx")
    run_model_test(model)
    data = input("Enter your password 32 bytes long: ")
    if len(data) != 32:
        exit("Invalid length")
    model = load("model.onnx")
    try:
        run_model(bytearray(data.encode()), model)
    except AssertionError:
        print("Wrong password")
