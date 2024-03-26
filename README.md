<Project_1day>
1) 파일 구성 & 경로 설정
Project_1day
└ Shell_1day
  └ Shell_1day.sin
  └ main.cpp
    └ Line 13 : string path = "~/Release/SSD_1day.exe "; // 경로 설정,  마지막 한 칸 공백
  └ nand.txt  // SSD 저장 장소
  └ result.txt  // Shell -> SSD 읽을 때 값을 임시 저장할 장소, Shell에서 읽어옵니다
└ SSD_1day
  └ main.cpp
  └ SSD_1day.sin
  └ Release
    └ SSD_1day.exe

2) 실행
   SSD_1day main.cpp 실행 -> SSD_1day.exe 생성 -> Shell_1day main.cpp 실행 -> 명령어 입력

<Project_2day>
1) 파일 구성 & 경로 설정
Project_2day
└ Shell_2day
  └ Shell_2day.sin
  └ main.cpp
    └ Line 13 : string path = "~/Release/SSD_2day.exe "; // 경로 설정,  마지막 한 칸 공백
    └ Line 15 : string file_search_path = "~/Project_2day/Shell_2day/";  // "*.log" 파일 찾기 위한 경로
  └ nand.txt
  └ result.txt
  └ latest.log // 명령어 입력시마다, [시간] 명령어  : 성공여부 저장
  └ buffer.txt // SSD 자체 버퍼 기능을 위한 임시 저장소
  └ until_날짜_시간.log // latest.log가 10KB 넘어가면 저장
  └ until_날짜_시간.zip // until_날짜_시간.log가 2개 만들어지면 압축(확장자만 변경)
  └ run_list.lst // runner 수행할 명령어
└ SSD_2day
  └ main.cpp
  └ SSD_2day.sin
  └ Release
    └ SSD_2day.exe

2) 실행
   SSD_1day main.cpp 실행 -> SSD_2day.exe 생성 -> Shell_2day main.cpp 실행 -> 명령어 입력

<명령어> // help 커맨드
"help                각 명령어 사용 방법 출력
"write n content     SSD의 n번에 content를 저장합니다.
"read n              SSD의 n번의 content를 불러옵니다.
"fullwrite content   SSD 전체에 conent가 쓰여집니다.
"fullread            SSD 전체 값을 화면에 출력합니다.
"exit                Shell 종료합니다.
"testapp1            fullwrite -> fullread 올바른지 확인합니다.
"testapp2            0~5번 LBA에 특정 값으로 30번 write 수행 및 OverWrite 수행 -> read시 정상여부 확인합니다.

Shell_2day.exe run_list.lst    run_list에 있는 runner 명령어 수행하여 문제 있는지 확인
