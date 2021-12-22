require "ruby2d"

$codeC_map=Hash.new
$codeA_to_codeC=Hash.new

#codeA hash to code C's index, only for 0-9
for i in 0..9
    $codeA_to_codeC[i]=i+16
end

#input the codeC mapping dict
file = File.open("code128C_mapping.txt")

i=0
file.each_line do |line|
    $codeC_map[i]=line
    i+=1
end
puts $codeC_map[101]

file.close

#generate code
def generateCode128c(s)
    if s==""
        puts "Error: input is null"
        return ""
    end
    result=$codeC_map[105]
    checkBit=105
    numbers = []

    #even length without code128A
    if s.length%2==0
        i=1
        s.chars.each_slice(2) do |slice|
            index=Integer(slice.join)
            checkBit+=index*i
            result+=$codeC_map[index]
            i+=1
        end
        result+=$codeC_map[checkBit%103]+$codeC_map[106]
        return result
    
    #odd length with code128A
    else
        i=1
        s[0..-2].chars.each_slice(2) do |slice|
            index=Integer(slice.join)
            checkBit+=index*i
            result+=$codeC_map[index]
            i+=1
        end
        result+=$codeC_map[101]+$codeC_map[$codeA_to_codeC[Integer(s[-1])]]
        checkBit+=101*i+$codeA_to_codeC[Integer(s[-1])]*(i+1)
        result+=$codeC_map[checkBit%103]+$codeC_map[106]
        return result
    end
end

#generate picture
def generatePicture(s)
    set background: 'white'
    code=generateCode128c(s)

    if code.length==0
        puts "Error: the generateCode function failed."
    end

    for i in 0..code.length-1
        if code[i]=='b'
            Rectangle.new(x:200+i*2,y:150,width:2,height:135,color:'black')
        elsif code[i]=='s'
            Rectangle.new(x:200+i*2,y:150,width:2,height:135,color:'white')
        end
    end

    Text.new(
        s,
        x: 150+(code.length-1), y: 300,
        size: 15,
        color: 'black',
    )
    show
end

generatePicture("690123634452")