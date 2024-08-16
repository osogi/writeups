$DNLIB_PATH = $PSScriptRoot + "\dnlib.dll"
$BINARY_PATH = $PSScriptRoot+"\pon.exe" 
$ORIGINAL_BINARY_PATH = $PSScriptRoot+ "\pon.exe" # rebuilding the binary breaks some things, for broken things we use the original binary
$OUT_BINARY_PATH = $BINARY_PATH + "-patched.exe"


$_ = [System.Reflection.Assembly]::LoadFile($DNLIB_PATH) 
$mod = [dnlib.DotNet.ModuleDefMD]::Load($BINARY_PATH)
$modclean = [dnlib.DotNet.ModuleDefMD]::Load($ORIGINAL_BINARY_PATH)


$CONSTS = @{ [Uint32]0x54814406 = "Input Flag: ";  [Uint32]0x28BAA60E = "322376503";  [Uint32]0x5A6F3B14 = "";  [Uint32]0x5F33AAD5 = "0";  [Uint32]0x0639E9F6 = "4";  [Uint32]0x280B428F = "0";  [Uint32]0x4E376A15 = "0";  [Uint32]0x352F802C = "4";  [Uint32]0x5346C655 = "a27019723705c0a8d30f500f954287fd0917798110d07c26a34db95720c35e64f54341909c72188b498dca71364c2a5caa46ccbe4be0b049eda8dec93cb61d24341354a1dcb2333366941553fbe3d07daf4fa9d8eb3d65708c6d2c18d62cd9f59b9362e2d80fb6b53130b0e913451a4e966121e31e2348d8eef33250774e73c025e265e3702340f3f4c3cb459c455c6ccb29d77b5882b69937fe1aa334a78c301c1a2f875583b43d9bd74ea3b89ebfe7069adef036404fb7e1198b8fecff443c9624816f855abd00e09f932c4cf3b8cc0c7e9a78654946d3a23f1b7cf46b2fe307ae616959326eed0562b61c46ec1d6f8bf4390272937fcafa178f4e05593265601be9710ae25f7fbcc7eca2e0a00a9c1beaf2a81185cc825ea75e38892d654925dc330b4b2249b8142b565df1ced881773dab2249c0cea9c8f6d7634bf0e9f3fa6408655b1700cf9aa6d23ff8957f0eaa712953a3c0d09ac71058b9ab35acdd17e117288642879110850b62f83cb13d07e7961e321c1c762ff5e5ab83029c7c9aeb7ea437db5c5d596b3f0c85564f6d6864fcc5353fdccdb7271093f2ca1a467065e70bb1a8db68c56c6f5940742d583212788743e825c4c2f4ef80ab6f125865ab4349582438e72ba51024770c0a54077ab7245dd26770336c7a8e0017781c8d9076b06873dd23f21405492e4cf803d4dc12fe9d475c0cb3e58f614860ebb13baeaad755eeb9cc2d8d3ed706919db9501f8df7ee778dbf42e5f463a6a7e371fcaae93768b1722dcefc0a06729388acfe35a4d08cbdfc996309a15fad7e62afa472fd4848d383a9d2a7b7db04b02dad623d6662b8f9564d3e8e5e7dbc9e02b58f66be440f005ae388fb224cae3feb4185dc4577cfaae780c2566bff77769a1083b22796eda8c3522a0cb9268fd30f9142b1fe1738a2e6e2c3dde787c70b4eda2bb6a22638ab7deb7cc6b6953d0e758e016c49d7b03915df31304acf997932d8863c8671eef947c3f2423a8aed2868bc051b394fbd6e54142799a48378ac9827379d327d5592ee38ac852b4207aede9bc569fdb7244e3acf0c3a533c441c8ec8128621a3908f193432ee174694a5ea455b01a0a84df44c1764edb9e6621aca210c56b6da4f748d5d8a5c9c3c4a34c925c964017b2fdf9f0fb03126f6fe3385b9ab1d3c0a3a12eddd2b13faed957d12cbb6fa616706240d424657a13fabaffc74c34b54f384d907fd9f6b93b8d1b2d7bcd6205b5e554fef4f26a4dd7375440bbc315148153fab431231d6d7e1d76635d1ff483d46e488aa5b4b78eb774131003066d96b6f4a6091cae5ee4641b289c9e4ac38314b9fc4fd8f468f00d675575f9c28f53a029fd723baa97a97b8913ac8d461bbdaec866d4ecff4a4a5911fdcf8d2873320a498c0fb58b2bb85a1fc1627ac15651abac85f4614321fc6c8117d5df3c8cab7bfa99909049b7823f947b1a2d4a79a50da5d94be0de87e5a97a640c3ee1854ada395d11549286d8d53d20e79d932a72cf98b9e44faa6eb3902b1ba99633f4aa8dc3da406a7028da027e22d288ef86d48b670641491655472ae83c2b5258ffbe21935e4cc1e0d82bb03fe7a25880d0c261085b3e22f397d4199b5470b538777dd1a58a7b29197af2ef18ec5dc0be90627ca83885f27b04465d13c05a5c9b438ef3dc64c00b40c8efaebdbdeb70b9ebd3921d96887e9a95d076c1bc84a58";  [Uint32]0x3AAD0EAB = "HAROSH";  [Uint32]0x39837F80 = "PLOH(N)";  [Uint32]0x3BDC1B60 = "";  [Uint32]0x627FA657 = "x2";  [Uint32]0x6ACE8291 = "32";  [Uint32]0x7285ED16 = "0";  [Uint32]0x58512FC7 = "0";  [Uint32]0x18CA0AF6 = "1";  [Uint32]0x1E18D083 = "1";  [Uint32]0x45757598 = "1";  [Uint32]0x2371C32D = "32";  [Uint32]0x75DFA1DC = "32";  [Uint32]0x1C29FCD8 = "32";  [Uint32]0x5BEEE396 = "0";  [Uint32]0x21407049 = "3";  [Uint32]0x3742C0FD = "1";  [Uint32]0x4DDD9901 = "1";  [Uint32]0x75FBC05A = "5"; }
$TINSTANCE2TMETHOD = @{ [Uint32]0x04000001 =  [Uint32]0x06000010; [Uint32]0x04000002 =  [Uint32]0x0A00000F; [Uint32]0x04000003 =  [Uint32]0x0A000010; [Uint32]0x04000004 =  [Uint32]0x0A000011; [Uint32]0x04000005 =  [Uint32]0x06000010; [Uint32]0x04000006 =  [Uint32]0x06000003; [Uint32]0x04000007 =  [Uint32]0x06000010; [Uint32]0x04000008 =  [Uint32]0x06000010; [Uint32]0x04000009 =  [Uint32]0x06000004; [Uint32]0x0400000A =  [Uint32]0x06000010; [Uint32]0x0400000B =  [Uint32]0x06000004; [Uint32]0x0400000C =  [Uint32]0x06000010; [Uint32]0x0400000D =  [Uint32]0x06000004; [Uint32]0x0400000E =  [Uint32]0x06000010; [Uint32]0x0400000F =  [Uint32]0x06000004; [Uint32]0x04000010 =  [Uint32]0x0A000017; [Uint32]0x04000011 =  [Uint32]0x06000009; [Uint32]0x04000012 =  [Uint32]0x0600000B; [Uint32]0x04000013 =  [Uint32]0x0A000018; [Uint32]0x04000014 =  [Uint32]0x06000002; [Uint32]0x04000015 =  [Uint32]0x0A000018; [Uint32]0x04000016 =  [Uint32]0x06000002; [Uint32]0x04000017 =  [Uint32]0x0A000019; [Uint32]0x04000018 =  [Uint32]0x06000010; [Uint32]0x04000019 =  [Uint32]0x06000004; [Uint32]0x0400001A =  [Uint32]0x06000010; [Uint32]0x0400001B =  [Uint32]0x0A00001A; [Uint32]0x0400001C =  [Uint32]0x06000010; [Uint32]0x0400001D =  [Uint32]0x0A00001B; [Uint32]0x0400001E =  [Uint32]0x06000010; [Uint32]0x0400001F =  [Uint32]0x0A00001B; [Uint32]0x04000020 =  [Uint32]0x0A00001C; [Uint32]0x04000021 =  [Uint32]0x0A00001D; [Uint32]0x04000022 =  [Uint32]0x06000010; [Uint32]0x04000023 =  [Uint32]0x06000010; [Uint32]0x04000024 =  [Uint32]0x0A000020; [Uint32]0x04000025 =  [Uint32]0x0A000021; [Uint32]0x04000026 =  [Uint32]0x0A000022; [Uint32]0x04000027 =  [Uint32]0x0600000B; [Uint32]0x04000028 =  [Uint32]0x06000010; [Uint32]0x04000029 =  [Uint32]0x06000004; [Uint32]0x0400002A =  [Uint32]0x0600000C; [Uint32]0x0400002B =  [Uint32]0x0600000A; [Uint32]0x0400002C =  [Uint32]0x06000010; [Uint32]0x0400002D =  [Uint32]0x06000003; [Uint32]0x0400002E =  [Uint32]0x06000010; [Uint32]0x0400002F =  [Uint32]0x06000004; [Uint32]0x04000030 =  [Uint32]0x06000010; [Uint32]0x04000031 =  [Uint32]0x06000004; [Uint32]0x04000032 =  [Uint32]0x0600000C; [Uint32]0x04000033 =  [Uint32]0x06000010; [Uint32]0x04000034 =  [Uint32]0x06000003; [Uint32]0x04000035 =  [Uint32]0x0600000A; [Uint32]0x04000036 =  [Uint32]0x06000010; [Uint32]0x04000037 =  [Uint32]0x06000004; [Uint32]0x04000038 =  [Uint32]0x0600000B; [Uint32]0x04000039 =  [Uint32]0x06000010; [Uint32]0x0400003A =  [Uint32]0x06000004; [Uint32]0x0400003B =  [Uint32]0x0600000C; [Uint32]0x0400003C =  [Uint32]0x06000010; [Uint32]0x0400003D =  [Uint32]0x06000004; [Uint32]0x0400003E =  [Uint32]0x0600000B; [Uint32]0x0400003F =  [Uint32]0x0600000A; [Uint32]0x04000040 =  [Uint32]0x0600000B; [Uint32]0x04000041 =  [Uint32]0x06000010; [Uint32]0x04000042 =  [Uint32]0x06000004; [Uint32]0x04000043 =  [Uint32]0x0600000C; [Uint32]0x04000044 =  [Uint32]0x0600000A; [Uint32]0x04000045 =  [Uint32]0x06000010; [Uint32]0x04000046 =  [Uint32]0x06000004; [Uint32]0x04000047 =  [Uint32]0x06000010; [Uint32]0x04000048 =  [Uint32]0x06000004; [Uint32]0x04000049 =  [Uint32]0x06000005; [Uint32]0x0400004A =  [Uint32]0x06000006; [Uint32]0x0400004B =  [Uint32]0x06000010; [Uint32]0x0400004C =  [Uint32]0x06000004; [Uint32]0x0400004D =  [Uint32]0x06000007; [Uint32]0x0400004E =  [Uint32]0x06000010; [Uint32]0x0400004F =  [Uint32]0x06000004; [Uint32]0x04000050 =  [Uint32]0x06000008; [Uint32]0x04000051 =  [Uint32]0x0600000D; [Uint32]0x04000052 =  [Uint32]0x0600000D; [Uint32]0x04000053 =  [Uint32]0x06000010; [Uint32]0x04000054 =  [Uint32]0x06000004; [Uint32]0x04000055 =  [Uint32]0x0A000023; [Uint32]0x04000056 =  [Uint32]0x0A000024; [Uint32]0x04000057 =  [Uint32]0x0A00002C; [Uint32]0x04000058 =  [Uint32]0x0A000023}
$NOPI = [dnlib.DotNet.Emit.Instruction]::Create([dnlib.DotNet.Emit.OpCodes]::Nop)
$launch2method = @{}




function renameAndInit_launch2method(){
    $used_names = @{}
    foreach ($tfield in $TINSTANCE2TMETHOD.Keys)
    {
        $tfunc = $TINSTANCE2TMETHOD[$tfield]

        $func = $mod.ResolveToken($tfunc)
        $field = $mod.ResolveToken($tfield)

        if($func -isnot  [dnlib.DotNet.MethodDef]){
             $func = $modclean.ResolveToken($tfunc)
        }

        $m =  $func.DeclaringType.Name.ToString() | Select-String -Pattern '(\w*)(.*?)$' 
        $pre = $m.Matches.Get(0).Groups[1].Value
        $new_name = $pre +"_"+ $func.Name.ToString()
        if($used_names.Contains($new_name)){
            $used_names[$new_name] =  $used_names[$new_name] + 1
            $new_name+= $used_names[$new_name].ToString()
        }else{
             $used_names[$new_name]=0
        }

        $class = $field.FieldType
        $td = $class.TypeDef

        $find = 0
    
        foreach ($m in $td.Methods){
            if($m.Name -notmatch "^((Invoke)|(\.cctor)|(\.ctor))$"){
                $find = 1
                $m.Name = "launch"
                $launch2method[$m]=$func
            }
        }
        if($find -eq 0){
            echo "Can't find function"
            exit
        }
        $td.Name = $new_name
    }
}

function replaceInstruction( [dnlib.DotNet.Emit.Instruction] $old, [dnlib.DotNet.Emit.Instruction] $new){
    $old.OpCode = $new.OpCode
    $old.Operand = $new.Operand
}

function createLdstr([string]$str){
    return [dnlib.DotNet.Emit.Instruction]::Create([dnlib.DotNet.Emit.OpCodes]::Ldstr, $str)
}

function deobfuscateConstFromDict([dnlib.DotNet.MethodDef]$meth){
    $dictFun = $mod.ResolveToken([uint32]0x06000010)
    
    $code = $meth.Body.Instructions
    for($i = 0; $i -lt $code.Count; $i++){
        $cur = $code[$i]
        if($cur.Operand -eq $dictFun){      
            $putarg = $code[$i-2]
            $str = $CONSTS[[uint32]$putarg.Operand]

            replaceInstruction $putarg $NOPI
            replaceInstruction ($code[$i]) (createLdstr($str))            
        }
        
    }
}

function deobfuscateDelegateCalls([dnlib.DotNet.MethodDef]$meth){
    $code = $meth.Body.Instructions
    for($i = 0; $i -lt $code.Count; $i++){
        $cur = $code[$i]
        if($cur.Operand -in $launch2method.Keys){          
            $cur.Operand = $launch2method[$cur.Operand]
            replaceInstruction ($code[$i-1]) $NOPI
        }
    }
}

function deobfuscateFull([dnlib.DotNet.MethodDef]$meth){
   #$meth.Body.KeepOldMaxStack = $true
   deobfuscateDelegateCalls($meth)
   deobfuscateConstFromDict($meth)
}



$tps = $mod.GetTypes()
$obfclasses = [ System.Array]::CreateInstance([dnlib.DotNet.TypeDef], 0)

foreach ($cl in $tps)
{
    $ns = $cl.ReflectionNamespace
    if($ns.Length -ne 0){
        $obfclasses += $cl
    }
}

renameAndInit_launch2method

foreach($cl in $obfclasses){
    foreach ($m in $cl.Methods){
        deobfuscateFull($m)
    }
}

echo ("Cleaned file: " + $OUT_BINARY_PATH)
$mod.Write($OUT_BINARY_PATH)
