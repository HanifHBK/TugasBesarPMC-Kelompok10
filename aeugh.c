#include <stdio.h>
#include <stdlib.h>
#include <math.h>
int i,j,temp,varcount,mincount,dccount,NumberOfEPI=0, remainingmin,remainingpi,NumberOfPI=0,PotEPINo=0,possibleepi=1,MinimumNo=0,Groupable=1;
int *minIndexDec, *dcIndexDec, **minBinary, ****Column, **PI_Index, **EPI_Index,*NumberCounter,*ReducedPIChart_X,**ReducedPIChart_Y,**ReducedPIChart, *For, **Potential_EPI,*NoOfPIForEPI;


void DecimalToBinary();
int OneCounter(int *binary, int NumberOfDigit);
int Combination(int n, int ColumnNo, int k);
int IsPowerOfTwo(int n);
int IsDontCare(int MT);
void ShowResult();
void Recursion_For_Loop(int m);


int main(){
    int k,l,m,n,x,y,LogicProbe;
    printf("Program untuk menyederhanakan persamaan boolean.\n");
    printf("Masukkan jumlah variabel persamaan: ");
    // jumlah variabel
    scanf("%d", &varcount);
    while(varcount <= 0){
        printf("Jumlah variabel tidak valid, jumlah variabel harus lebih besar dari 0.\n");
        printf("Masukkan jumlah variabel persamaan: ");
        scanf(" %d", &varcount);
    }

    // jumlah minterm
    printf("Masukkan jumlah minterm (termasuk don't care) pada persamaan: ");
    scanf(" %d", &mincount);
    while(mincount > pow(2, varcount) || mincount <= 0){
        if(mincount == 0){
            printf("Tidak terdapat minterm pada persamaan, maka bentuk paling sederhana dari persamaan tersebut adalah : \nFalse");
            return 0;
        }
        printf("Jumlah minterm tidak valid, minterm harus memiliki jumlah <= 2 pangkat banyaknya variabel\n");
        printf("Masukkan jumlah minterm pada persamaan: ");
        scanf(" %d", &mincount);
    }

    // jumlah don't care
    printf("Masukkan jumlah Don't Care pada persamaan: ");
    scanf(" %d", &dccount);
    while(dccount >= mincount || dccount < 0){
        printf("Jumlah don't care tidak boleh lebih banyak daripada minterm!\n");
        printf("Masukkan jumlah Don't Care pada persamaan: ");
        scanf(" %d", &dccount);
    }


    // input minterm
    minIndexDec = (int*) malloc(mincount*sizeof(int));
    for(i =0; i < mincount; i++){
        printf("Masukkan minterm ke-%d (berurut dari rendah):", i+1);
        scanf(" %d", &minIndexDec[i]);
        if(minIndexDec[i] < 0){
            printf("Minterm tidak boleh negatif, masukkan kembali nilai minterm!\n");
            i--;
        } else if(minIndexDec[i] >= pow(2, varcount)){
            printf("Minterm harus kurang dari 2 pangkat jumlah variabel! Masukkan kembali nilai minterm\n");
            i--;
        } else if(i != 0 && minIndexDec[i] <= minIndexDec[i-1]){
            printf("Minterm tidak berurut, masukkan kembali nilai minterm!\n");
            i--;
        }
    }

    // input don't care
    dcIndexDec = (int*) malloc(dccount*sizeof(int));
    for(i =0; i < dccount; i++){
        printf("Masukkan Don't Care ke-%d (berurut dari rendah):", i+1);
        scanf(" %d", &dcIndexDec[i]);
        if(minIndexDec[i] < 0){
            printf("Don't Care tidak boleh negatif, masukkan kembali nilai Don't Care!\n");
            i--;
        } else if(dcIndexDec[i] >= pow(2, varcount)){
            printf("Don't Care harus kurang dari 2 pangkat jumlah variabel! Masukkan kembali nilai Don't Care.\n");
            i--;
        } else if(i != 0 && dcIndexDec[i] <= dcIndexDec[i-1]){
            printf("Don't Care tidak berurut, masukkan kembali nilai Don't Care!\n");
            i--;
        }
    }
    //ubah minterm ke binary

    minBinary=(int **)malloc(mincount*sizeof(int*));
    for(i=0;i<=mincount;i++){
        minBinary[i]=(int *)malloc((varcount+4)*sizeof(int));
    }
    DecimalToBinary();
    for(i=0;i<mincount;i++){
        minBinary[i][varcount]=OneCounter(minBinary[i],varcount); // berisi jumlah angka 1
        minBinary[i][varcount+1]=0; // 0 artinya belum dikelompokkan, 1 sudah di kelompokkan
        minBinary[i][varcount+2]=minIndexDec[i]; // original minterm
        minBinary[i][varcount+3]=minIndexDec[i]; // minterm - minterm setelah digabung
    }

    // Inisiasi column untuk dilakukan minimisasi
    Column=(int ****)malloc((varcount+1)*sizeof(int***));
    for(i=0;i<varcount+1;i++){ //varcount+1 karena jumlah angka 1 dari 0 sampai varcount
        Column[i]=(int ***)malloc((varcount+1-i)*sizeof(int**)); 
    }
    for(i=0;i<varcount+1;i++){
        for(j=0;j<varcount+1-i;j++){
            Column[i][j]=(int**)malloc(Combination(varcount,i,j)*sizeof(int*)); 
            for(k=0;k<Combination(varcount,i,j);k++){
                Column[i][j][k]=NULL;
            }
        }
    }

    for(i=0;i<varcount+1;i++){
        for(j=0,k=0;j<mincount;j++){
            if(minBinary[j][varcount]==i){
                Column[0][i][k++]=minBinary[j];
                /* Prepare the first grouping column */
            }
        }
    }

    /***********Perform the grouping***********/
    for(i=0;i<varcount+1;i++){
        if(Groupable){
            Groupable=0;
            for(j=0;j<varcount-i;j++){
                int p,position;
                m=0;
                for(k=0;k<Combination(varcount,i,j);k++)
                if(Column[i][j][k]!=NULL){
                    for(l=0;l<Combination(varcount,i,j+1);l++){
                        if(Column[i][j+1][l]!=NULL && Column[i][j+1][l][varcount+2+i]>Column[i][j][k][varcount+2+i] && IsPowerOfTwo(Column[i][j+1][l][varcount+2+i]-Column[i][j][k][varcount+2+i])){
                            LogicProbe=0-i;
                            for(n=1;n<=i;n++)
                            for(p=1;p<=i;p++)
                            if(Column[i][j+1][l][varcount+1+n]==Column[i][j][k][varcount+1+p]){
                                LogicProbe++;
                            }
                            if(LogicProbe==0){
                                Groupable=1;
                                Column[i][j][k][varcount+1]=1;
                                Column[i][j+1][l][varcount+1]=1;
                                Column[i+1][j][m]=(int *)malloc((varcount+4+i+pow(2,i+1))*sizeof(int));
                                for(n=0;n<=varcount+1+i;n++){
                                    Column[i+1][j][m][n]=Column[i][j][k][n];
                                }
                                Column[i+1][j][m][varcount+3+i]=Column[i][j][k][varcount+2+i];
                                for(n=varcount+4+i;n<varcount+4+i+pow(2,i+1);n++)
                                Column[i+1][j][m][n]=0;
                                position=log((Column[i][j+1][l][varcount+2+i]-
                                Column[i][j][k][varcount+2+i]))/log(2);
                                Column[i+1][j][m][varcount-1-position]=2;
                                Column[i+1][j][m][varcount+1]=0;
                                Column[i+1][j][m][varcount+2+i]=position;
                                for(p=0;p<pow(2,i);p++){
                                    Column[i+1][j][m][varcount+4+i+p]=Column[i][j][k][varcount+3+i+p];
                                }
                                for(p=pow(2,i);p<pow(2,i+1);p++){
                                    Column[i+1][j][m][varcount+4+i+p]=Column[i][j+1][l][varcount+3+i+p-(int)pow(2,i)];
                                }
                                m++;
                            }
                        }
                    }
                }
            }
        }
    }

    // number counter
    NumberCounter=(int *)malloc(pow(2,varcount)*sizeof(int));
    for(i=0;i<pow(2,varcount);i++)
    NumberCounter[i]=0;

    // menyimpan prime implicant (duplikat tidak disimpan)
    PI_Index=(int **)malloc(mincount*sizeof(int*));
    for(i=0;i<mincount;i++){
        PI_Index[i]=(int *)malloc(3*sizeof(int));
    }
    for(i=0;i<varcount+1;i++)
    for(j=0;j<varcount+1-i;j++)
    for(k=0;k<Combination(varcount,i,j);k++){
        if(Column[i][j][k]!=NULL && Column[i][j][k][varcount+1]==0){
            LogicProbe=0-pow(2,i); /*LogicProbe is used to check whether this PI is a duplicate*/
            for(l=k-1;l>=0;l--)
            if(LogicProbe!=0){
                LogicProbe=0-pow(2,i);
                for(m=0;m<pow(2,i);m++)
                for(n=0;n<pow(2,i);n++)
                if(Column[i][j][l][varcount+3+i+m]==Column[i][j][k][varcount+3+i+n]){
                    LogicProbe++;
                }
            }
            if(LogicProbe!=0){
                PI_Index[NumberOfPI][0]=i;
                PI_Index[NumberOfPI][1]=j;
                PI_Index[NumberOfPI][2]=k;
                NumberOfPI++;
                for(l=0;l<pow(2,i);l++){
                    NumberCounter[Column[i][j][k][varcount+3+i+l]]++;
                }
            }
        }
    }


    //menghilangkan minterm yang ada don't care
    for(i=0;i<dccount;i++)
    NumberCounter[dcIndexDec[i]]=0;
    EPI_Index=(int **)malloc(mincount*sizeof(int*));


    /*
    memilih PI yang nilai mintermnya hanya muncul sekali dan dipilih 
    sebagai epi dan minterm yang ada pada pi tersebut dihilangkan
    */
    for(i=0;i<pow(2,varcount);i++)
    if(NumberCounter[i]==1)
    for(j=0;j<NumberOfPI;j++)
    for(k=0;k<pow(2,PI_Index[j][0]);k++){
        if(Column[PI_Index[j][0]][PI_Index[j][1]][PI_Index[j][2]][varcount+3+PI_Index[j][0]+k]==i){
            EPI_Index[NumberOfEPI]=PI_Index[j];
            for(l=0;l<pow(2,PI_Index[j][0]);l++)
            NumberCounter[Column[PI_Index[j][0]][PI_Index[j][1]][PI_Index[j][2]][varcount+3+PI_Index[j][0]+l]]=0;
            NumberOfEPI++;
            k=pow(2,PI_Index[j][0]);
        }
    }


    // membuat tabel pi yang belum dipilih
    remainingmin=0;
    for(i=0;i<pow(2,varcount);i++)
    if(NumberCounter[i]!=0)
    remainingmin++;
    ReducedPIChart_X=(int *)malloc(remainingmin*sizeof(int));
    for(i=0;i<remainingmin;i++)
    ReducedPIChart_X[i]=-1;
    ReducedPIChart_Y=(int **)malloc(NumberOfPI*sizeof(int*));
    for(i=0;i<NumberOfPI;i++)
    ReducedPIChart_Y[i]=NULL;
    ReducedPIChart=(int **)malloc(remainingmin*sizeof(int*));

    // first row, menandakan remaining minterm yang belum ada pada epi
    for(i=0,j=0;j<pow(2,varcount);j++)
    if(NumberCounter[j]!=0){
        ReducedPIChart_X[i]=j;
        i++;
    }

    // first column, menandakan remaining pi yang belum masuk ke epi
    remainingpi=0;
    for(i=0;i<NumberOfPI;i++)
    for(j=0;j<pow(2,PI_Index[i][0]);j++){
        if(NumberCounter[Column[PI_Index[i][0]][PI_Index[i][1]][PI_Index[i][2]][varcount+3+PI_Index[i][0]+j]]!=0){
            j=pow(2,PI_Index[i][0]);
            ReducedPIChart_Y[remainingpi]=PI_Index[i];
            remainingpi++;
        }
    }


    /***********ReducedPIChart[i][j] represent the information of Reduced PI Chart('1'
    means picked, '0' means unpicked)***********/
    if(remainingpi!=0){
        for(i=0;i<remainingmin;i++)
        ReducedPIChart[i]=(int *)malloc(remainingpi*sizeof(int));
        for(i=0;i<remainingmin;i++)
        for(j=0;j<remainingpi;j++)
        ReducedPIChart[i][j]=0;
        for(i=0;i<remainingpi;i++)
        for(j=0;j<pow(2,ReducedPIChart_Y[i][0]);j++)
        for(k=0;k<remainingmin;k++)
        if(Column[ReducedPIChart_Y[i][0]][ReducedPIChart_Y[i][1]][ReducedPIChart_Y[i][2]][varcount+3+ReducedPIChart_Y[i][0]+j]==ReducedPIChart_X[k]){
            ReducedPIChart[k][i]=1;
        }

        //memilih epi dari remaining pi
        For=(int *)malloc(remainingmin*sizeof(int)); /* For[i] will be used in the function 'Recursion_For_Loop(int m)' */
        for(i=0;i<remainingmin;i++){
            For[i]=-1;
        }
        for(i=0;i<remainingmin;i++)
        possibleepi=possibleepi*NumberCounter[ReducedPIChart_X[i]];
        Potential_EPI=(int **)malloc(possibleepi*sizeof(int*));
        for(i=0;i<possibleepi;i++){
            Potential_EPI[i]=(int *)malloc(remainingmin*sizeof(int));
        }
        Recursion_For_Loop(remainingmin-1);
        NoOfPIForEPI=(int *)malloc(possibleepi*sizeof(int)); 
        /*
        NoOfPIForEPI[i] will count how many PIs are in each combination that covers all
        minterms */

        for(i=0;i<possibleepi;i++)
        NoOfPIForEPI[i]=0;
        for(i=0;i<possibleepi;i++)
        for(j=0;j<remainingmin;j++)
        if(Potential_EPI[i][j]!=-1){
            NoOfPIForEPI[i]++;
            for(k=j+1;k<remainingmin;k++)
            if(Potential_EPI[i][k]==Potential_EPI[i][j])
            Potential_EPI[i][k]=-1;
        }
        
        for(i=1;i<possibleepi;i++)
        if(NoOfPIForEPI[i]<NoOfPIForEPI[MinimumNo])
        MinimumNo=i;
        for(i=0;i<remainingmin;i++)
        if(Potential_EPI[MinimumNo][i]!=-1)
        EPI_Index[NumberOfEPI++]=ReducedPIChart_Y[Potential_EPI[MinimumNo][i]];

        // print final result
        printf("\nPersamaan boolean yang telah disederhanakan adalah:\n\n");
        printf("\n ");
        for(x=0;x<NumberOfEPI;x++){
            for(y=0;y<varcount;y++){
                if(Column[EPI_Index[x][0]][EPI_Index[x][1]][EPI_Index[x][2]][y]==1)
                printf("%c",65+y);
                else if(Column[EPI_Index[x][0]][EPI_Index[x][1]][EPI_Index[x][2]][y]==0)
                printf("%c'",65+y);
            }
            if(x<NumberOfEPI-1)
            printf("+");
        }
        printf("\n\nPress any key to exit...");
        scanf("%d",&i);
        return 0;
    } else {
        printf("\n\nPersamaan boolean yang telah disederhanakan adalah:\n\n");
        printf("\n ");
        for(x=0;x<NumberOfEPI;x++){
            for(y=0;y<varcount;y++){
                if(Column[EPI_Index[x][0]][EPI_Index[x][1]][EPI_Index[x][2]][y]==1)
                printf("%c",65+y);
                else if(Column[EPI_Index[x][0]][EPI_Index[x][1]][EPI_Index[x][2]][y]==0)
                printf("%c'",65+y);
            }
            if(x<NumberOfEPI-1)
            printf("+");
            }
        printf("\n\nPress any key to exit...");
        scanf("%d",&i);
        return 0;
    }
}


// Fungsi tambahan
int IsDontCare(int MT){
    int i;
    for(i=0;i<dccount;i++)
    if(MT==dcIndexDec[i])
    return 1;
    return 0;
}

void DecimalToBinary(){
    int i,j,dividend;
    for(i=0;i<mincount;i++){
        dividend=minIndexDec[i];
        for(j=varcount-1;j>=0;j--){
            minBinary[i][j]=dividend%2;
            dividend=dividend/2;
        }
    }
}


int OneCounter(int *binary, int NumberOfDigit){
    int i,count=0;
    for(i=0;i<=NumberOfDigit-1;i++){
        if(binary[i]==1)
        count++;
    }
    return count;
}


int Combination(int n, int ColumnNo, int k){
    int Comb,i,NtoK=1,Kto1=1;
    for(i=n;i>=n-k+1-ColumnNo;i--){
    NtoK=i*NtoK;
    }
    for(i=k;i>=1;i--){
    Kto1=i*Kto1;
    }
    Comb=NtoK/Kto1;
    return Comb;
}


int IsPowerOfTwo(int n){
return(floor(log(n)/log(2))==(log(n)/log(2)));
}


void Recursion_For_Loop(int m){
    int n=m;
    for(For[n]=0;For[n]<remainingpi;For[n]++){
        if(ReducedPIChart[remainingmin-1-n][For[n]]){
            if(n>0){
                m=n;
                m--;
                Recursion_For_Loop(m);
            } else if(n==0){
                for(i=0;i<remainingmin;i++){
                    Potential_EPI[PotEPINo][i]=For[remainingmin-1-i];
                }
                PotEPINo++;
            }
        }
    }
}