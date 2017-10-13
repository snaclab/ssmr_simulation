/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package simulation;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Random;
import jxl.Sheet;
import jxl.Workbook;
import jxl.read.biff.BiffException;
import jxl.write.Label;
import jxl.write.WritableSheet;
import jxl.write.WritableWorkbook;
import jxl.write.WriteException;

/**
 *  還沒寫    if < user to BaseStation
 * @author chung
 */
public class Simulation_for_case1 {

    private final static String Filename = "case1_two_hops.xls";
    private final static String Filename2 = "case1_normal.xls";
    private final static String Filename3 = "example5.xls";
    private final static String sheet_name = "case1";
    private final static int datasize = 100;
    private static int number_of_data =7; // total we have relay nodes
    private static int number_of_path =7; // select how number of nodes to relay
    private static int available_nodes = 0;

    static ArrayList<Double> no_relay = new ArrayList<Double>();
    static ArrayList<Double> single_node_relay_random = new ArrayList<Double>();
    static ArrayList<Double> single_node_relay_best_path = new ArrayList<Double>();
    static ArrayList<Double> our_not_optimal_scheme = new ArrayList<Double>();
    static ArrayList<Double> our_scheme = new ArrayList<Double>();
    static ArrayList<Double> greedy_scheme = new ArrayList<Double>();

    static ArrayList<Double> result_for_rate = new ArrayList<Double>();
    static ArrayList<Structure_for_twohops> Linked_path = new ArrayList<Structure_for_twohops>();
    static ArrayList<Structure_for_twohops> Random_order_Linked_path = new ArrayList<Structure_for_twohops>();
    static ArrayList<Structure_for_twohops> Greedy_order_Linked_path = new ArrayList<Structure_for_twohops>();
    static double usertobasestation = 0;
    static int looptime =60;
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) throws IOException {

        Simulation_for_case1 call = new Simulation_for_case1();
        //call.create_file();

        //for(int i =1;i<number_of_data+1;i++){
        //number_of_data = i;
        for (int j =1; j < 2; j += 1) {
            for (int i = 0; i < looptime; i++) {
                //call.readTxT(("C:/Users/chung/Desktop/模擬環境/"+"S1_11/"+"S1_"+String.valueOf(i)+".txt"),String.valueOf(j));
                //call.readTxT(("C:/Users/chung/Desktop/模擬環境/"+"S2_210/"+"S2_210_"+String.valueOf(i)+".txt"),String.valueOf(j));
                //call.readTxT(("C:/Users/chung/Desktop/模擬環境/"+"2S/"+"S2_100_"+String.valueOf(i)+".txt"),String.valueOf(j));
                call.readTxT(("C:/Users/chung/Desktop/模擬環境/"+"S5/"+"S5_180_"+String.valueOf(i)+".txt"),String.valueOf(j));
              
//                call.readTxT(("C:/Users/chung/Desktop/模擬環境/"+"S5/"+"S5_150/"+"S5_150_"+String.valueOf(i)+".txt"),String.valueOf(j));
                
                call.load_data_toArray("example" + j + ".xls", "example");
                call.sort_arrayList();
                //********************************************
                System.out.print("available nodes: "+available_nodes+"\n");
                number_of_path=available_nodes;
                //********************************************
                call.all_method(datasize);
                result_for_rate.clear();
                Linked_path.clear();
                Random_order_Linked_path.clear();
                Greedy_order_Linked_path.clear();
                //********************************************
                available_nodes=0;
            }   
            call.write_to_excel("180" +"_"+ j);

            
//            no_relay.clear();
//            single_node_relay_random.clear();
//            single_node_relay_best_path.clear();
//            our_not_optimal_scheme.clear();
//            our_scheme.clear();
        }
        //}
        /*
        for(int i =1;i<16;i++){
        call.load_data_toArray(Filename,String.valueOf(i));
        call.sort_arrayList();
        call.all_method(1);
        result_for_rate.clear();
        Linked_path.clear();
        Random_order_Linked_path.clear();
        }*/

    }

    public void readTxT(String FileName, String number) throws FileNotFoundException, IOException {
        FileReader fr = new FileReader(FileName);
        String temp = "";
        BufferedReader br = new BufferedReader(fr);
        int count = 1;
        try {
            Label label;
            WritableWorkbook workbook = Workbook.createWorkbook(new File(
                    "example" + number + ".xls"));
            WritableSheet sheet = workbook.createSheet("example", 0);
            while (br.ready()) {
                temp = br.readLine();
                //System.out.println(temp.split("\t")[0] +" "+temp.split("\t")[1]);
                label = new Label(0, count, String.valueOf(count));
                sheet.addCell(label);
                if (count != 1) {
                    label = new Label(1, count, temp.split("\t")[0]);
                    sheet.addCell(label);
                }
                label = new Label(2, count, temp.split("\t")[1]);
                sheet.addCell(label);

                count++;
            }

            workbook.write();
            workbook.close();
        } catch (IOException e) {
            e.printStackTrace();
        } catch (WriteException e) {
            e.printStackTrace();
        }

        fr.close();
    }

    public void all_method(int data_size) {
        no_relay(data_size); // OK
        single_node_relay_random(data_size); //OK
        single_node_relay_best_path(data_size); //OK
        result_for_rate.clear();
        multi_node_relay_not_optimal(number_of_path, data_size);
        result_for_rate.clear();
        multi_node_relay_optimal(number_of_path, data_size); // OK
        result_for_rate.clear();
        greedy_algo(number_of_path, data_size);
    }

    private void no_relay(int data_size) {
        no_relay.add((double) data_size / usertobasestation);
        //System.out.print("no relay:" + (double) data_size / usertobasestation + "\n");
    }

    private void single_node_relay_random(int data_size) {
        Random ran = new Random();
        int random = ran.nextInt(number_of_data);
//        System.out.print(" "+Linked_path.get(random).get_first()+"\n");
//        System.out.print(" "+Linked_path.get(random).get_second()+"\n");
        single_node_relay_random.add(((double) data_size / Linked_path.get(random).get_first()
                + (double) data_size / Linked_path.get(random).get_second()));
//        System.out.print("single_node_relay_random: " + ((double) data_size / Linked_path.get(random).get_first()
//                + (double) data_size / Linked_path.get(random).get_second()) + "\n");
    }

    private void single_node_relay_best_path(int data_size) {
        double max = usertobasestation;
        double c = 0;
        int index_max = 0;
        for (int i = 0; i < Linked_path.size(); i++) {
            c = (double) (Linked_path.get(i).get_first() * Linked_path.get(i).get_second())
                    / (double) (Linked_path.get(i).get_first() + Linked_path.get(i).get_second());
            if (max < c) {
                index_max = i;
                max = c;
            }
        }

        single_node_relay_best_path.add((double) data_size / max);
       // System.out.print("single_node_relay_best_path: " + (double) data_size / max + "\n");
    }
    private void greedy_algo(int number_of_nodes, int data_size){
        greedy_order();
        cal_proportion(number_of_nodes, Greedy_order_Linked_path);
        greedy_scheme.add(call_total_time(data_size, Greedy_order_Linked_path));
        System.out.print("greedy_scheme:" + call_total_time(data_size, Greedy_order_Linked_path) + "\n");
    
    }
    
    private void greedy_order(){
        for(int i =0;i<Greedy_order_Linked_path.size();i++)
            for(int j =i;j<Greedy_order_Linked_path.size();j++){
                if (greedy_judgyment(i) < greedy_judgyment(j)) {
                    Structure_for_twohops tmp = new Structure_for_twohops(
                            Greedy_order_Linked_path.get(i).get_first(), Greedy_order_Linked_path.get(i).get_second());

                    Greedy_order_Linked_path.set(i, new Structure_for_twohops(
                            Greedy_order_Linked_path.get(j).get_first(), Greedy_order_Linked_path.get(j).get_second()));

                    Greedy_order_Linked_path.set(j, tmp);
                }
        }
    }
    private double greedy_judgyment(int i){
    return Greedy_order_Linked_path.get(i).get_first()*(Greedy_order_Linked_path.get(i).get_second()+usertobasestation)
            /(Greedy_order_Linked_path.get(i).get_first()+Greedy_order_Linked_path.get(i).get_second());
    }

    private void multi_node_relay_not_optimal(int number_of_nodes, int data_size) {
        random_linked_order();
        cal_proportion(number_of_nodes, Random_order_Linked_path);
        our_not_optimal_scheme.add(call_total_time(data_size, Random_order_Linked_path));
        System.out.print("our_not_optimal_scheme:" + call_total_time(data_size, Random_order_Linked_path) + "\n");
    }

    private void multi_node_relay_optimal(int number_of_nodes, int data_size) {
        cal_proportion(number_of_nodes, Linked_path);
        our_scheme.add(call_total_time(data_size, Linked_path));
        System.out.print("our_scheme:" + call_total_time(data_size, Linked_path) + "\n");
    }

    private double call_total_time(int data_size, ArrayList<Structure_for_twohops> array) {
        double data_rate = result_for_rate.get(0) / sum_of_denominator(array);
//        System.out.print("\n1 "+result_for_rate.get(0));
//        System.out.print("\n2 "+sum_of_denominator(array));
//        System.out.print("\n2 "+data_rate);
//        System.out.print("\n3 "+1f/array.get(0).get_first());
//        System.out.print("\n4 "+1f/array.get(0).get_second()+"\n");
        return data_size * (double) result_for_rate.get(0) / (double) sum_of_denominator(array) * 
                (1d / array.get(0).get_first() + 1d / array.get(0).get_second());
    }

    private double sum_of_denominator(ArrayList<Structure_for_twohops> array) {
        double sum_of_denominator = 0;
        for (int i = 0; i < result_for_rate.size(); i++) {
            sum_of_denominator += result_for_rate.get(i);
        }
        return sum_of_denominator + result_for_rate.get(number_of_path - 1) / array.get(number_of_path - 1).get_second() * usertobasestation;
    }

    public void cal_proportion(int number_of_nodes, ArrayList<Structure_for_twohops> array) {  //計算資料分配的比例。
        for (int i = 0; i < number_of_nodes; i++) { //from 0 to 24
            result_for_rate.add(cal_rate_of_linkedpath(i, array));
        }
    }

    private double cal_rate_of_linkedpath(int order, ArrayList<Structure_for_twohops> array) {
        double S_add_F = 1.0;
        double S_multiply_F = 1.0;
        for (int i = order; i < number_of_path-1 ; i++) {
            S_add_F = S_add_F * first_add_second(array.get(i + 1).get_first(), array.get(i + 1).get_second());
      //System.out.print("("+array.get(i+1).get_first()+"+"+array.get(i+1).get_second()+")");
        }
        for (int i = 1; i <= order; i++) {
            S_multiply_F = S_multiply_F * array.get(i).get_first();
            //System.out.print("("+array.get(i).get_first()+")");
        }
        //System.out.print("\n"+"-----------------------------"+"\n");

       //System.out.print(S_add_F+" "+S_multiply_F+" "+S_add_F*S_multiply_F+"\n");
        return S_add_F * S_multiply_F * array.get(order).get_second();
    }

    private double first_add_second(double first, double second) {
        return first + second;
    }

    public void load_data_toArray(String Filename, String sheet_name) {
        try {
            Workbook workbook = Workbook.getWorkbook(new File(Filename));
            Sheet sheet = workbook.getSheet(sheet_name);

            usertobasestation = Double.valueOf(sheet.getCell(2, 1).getContents()) ;
            for (int j = 0; j < number_of_data; j++) {
                Linked_path.add(new Structure_for_twohops(
                        (Double.valueOf(sheet.getCell(1, 2 + j).getContents()) ),
                        (Double.valueOf(sheet.getCell(2, 2 + j).getContents())) ));
                Random_order_Linked_path.add(new Structure_for_twohops(
                        (Double.valueOf(sheet.getCell(1, 2 + j).getContents()) ),
                        (Double.valueOf(sheet.getCell(2, 2 + j).getContents())) ));
                Greedy_order_Linked_path.add(new Structure_for_twohops(
                        (Double.valueOf(sheet.getCell(1, 2 + j).getContents()) ),
                        (Double.valueOf(sheet.getCell(2, 2 + j).getContents())) ));
            }

            workbook.close();
        } catch (BiffException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void random_linked_order() {

        Random random = new Random();
        if (number_of_data > 1) {
            for (int i = 0; i < number_of_data; i++) { // 這個迴圈也可以只跑到result.length/2

                int index = random.nextInt(number_of_data - 1);

                //交換 i 跟index的原素
                Structure_for_twohops tmp = new Structure_for_twohops(
                        Random_order_Linked_path.get(index).get_first(), Random_order_Linked_path.get(index).get_second());
                Random_order_Linked_path.set(index, new Structure_for_twohops(
                        Random_order_Linked_path.get(i).get_first(), Random_order_Linked_path.get(i).get_second()));
                Random_order_Linked_path.set(i, tmp);

            }
            for (int i = 0; i < Random_order_Linked_path.size(); i++) {
                if(Random_order_Linked_path.get(i).get_first()<=usertobasestation){
                    Random_order_Linked_path.remove(i);
                    i=i-1;
                }
            }
                System.out.print("N:"+ String.valueOf(usertobasestation)+"\n");
            
        }
        //System.out.print("\n");
    }

    public void sort_arrayList() {
        for (int i = 0; i < Linked_path.size(); i++) { // 全部排序
            for (int j = i; j < Linked_path.size(); j++) {
                if (Linked_path.get(i).get_first() < Linked_path.get(j).get_first()) {
                    Structure_for_twohops tmp = new Structure_for_twohops(
                            Linked_path.get(i).get_first(), Linked_path.get(i).get_second());

                    Linked_path.set(i, new Structure_for_twohops(
                            Linked_path.get(j).get_first(), Linked_path.get(j).get_second()));

                    Linked_path.set(j, tmp);
                }
            }
        }
        for (int i = 0; i < Linked_path.size(); i++) { //如果有兩個的第一個hop data rate 相等， 小的在前面。
            for (int j = i; j < Linked_path.size(); j++) {
                if (Linked_path.get(i).get_first() == Linked_path.get(j).get_first()) {
                    if (Linked_path.get(i).get_second() < Linked_path.get(j).get_second()) {
                        Structure_for_twohops tmp = new Structure_for_twohops(
                                Linked_path.get(i).get_first(), Linked_path.get(i).get_second());

                        Linked_path.set(i, new Structure_for_twohops(
                                Linked_path.get(j).get_first(), Linked_path.get(j).get_second()));

                        Linked_path.set(j, tmp);
                        i = 0;
                    }
                }
            }
        }
        for (int z = 0; z < Linked_path.size(); z++) { // 
            if (Linked_path.get(z).get_first() >= usertobasestation) {
                available_nodes++;
            }
        }
    }

    public void create_file() {
        try {
            Label label;
            double GtoUser, GtoBS;
            WritableWorkbook workbook = Workbook.createWorkbook(new File(
                    "case1_two_hops" + ".xls"));

            Random random = new Random();
            for (int f = 1; f < 16; f++) {
                WritableSheet sheet = workbook.createSheet(String.valueOf(f), 0);
                sheet.addCell(new Label(2, 1, String.valueOf(1)));
                sheet.addCell(new Label(0, 1, "User"));
                sheet.addCell(new Label(1, 0, "F"));
                sheet.addCell(new Label(2, 0, "S"));
                for (int i = 1; i < 16; i++) {
                    if (i <= f) {
                        GtoUser = random.nextInt(5) + 25;
                        GtoBS = random.nextInt(5) + 10;
                    } else {
                        GtoUser = random.nextInt(5) + 10;
                        GtoBS = random.nextInt(5) + 25;
                    }

                    label = new Label(0, i + 1, String.valueOf(i));
                    sheet.addCell(label);
                    label = new Label(1, i + 1, String.valueOf(GtoUser / 10d));
                    sheet.addCell(label);
                    label = new Label(2, i + 1, String.valueOf(GtoBS / 10d));
                    sheet.addCell(label);
                }
            }

            workbook.write();
            workbook.close();

        } catch (IOException e) {
            e.printStackTrace();
        } catch (WriteException e) {
            e.printStackTrace();
        }
    }

    private double average(ArrayList<Double> a, int i, int j) {
        double temp = 0.0;
        for (int x = i; x < j; x++) {
            temp += a.get(x);
        }

        return temp / looptime;
    }

    public void write_to_excel(String filename) {
        try {
            Label label;
            WritableWorkbook workbook = Workbook.createWorkbook(new File(
                    filename + ".xls"));
            WritableSheet sheet = workbook.createSheet("result_example", 0);
            label = new Label(0, 0, "no_relay");
            sheet.addCell(label);
            label = new Label(1, 0, "single_node_relay_random");
            sheet.addCell(label);
            label = new Label(2, 0, "single_node_relay_best_path");
            sheet.addCell(label);
            label = new Label(3, 0, "our_not_optimal_scheme");
            sheet.addCell(label);
            label = new Label(4, 0, "our_scheme");
            sheet.addCell(label);
            label = new Label(5, 0, "greedy_scheme");
            sheet.addCell(label);
            label = new Label(6, 10, String.valueOf(number_of_path));
            sheet.addCell(label);
            for (int i = 0; i < our_scheme.size(); i++) {
                label = new Label(0, i + 1, String.valueOf(no_relay.get(i)));
                sheet.addCell(label);
                label = new Label(1, i + 1, String.valueOf(single_node_relay_random.get(i)));
                sheet.addCell(label);
                label = new Label(2, i + 1, String.valueOf(single_node_relay_best_path.get(i)));
                sheet.addCell(label);
                label = new Label(3, i + 1, String.valueOf(our_not_optimal_scheme.get(i)));
                sheet.addCell(label);
                label = new Label(4, i + 1, String.valueOf(our_scheme.get(i)));
                sheet.addCell(label);
                label = new Label(5, i + 1, String.valueOf(greedy_scheme.get(i)));
                sheet.addCell(label);
                if ((i + 1) % looptime == 0 && (i + 1) >= looptime) {
                    label = new Label(7, (i + 1) / looptime, String.valueOf(average(no_relay, i-looptime+1, i)));
                    sheet.addCell(label);
                    label = new Label(8, (i + 1) / looptime, String.valueOf(average(single_node_relay_random, i-looptime+1, i)));
                    sheet.addCell(label);
                    label = new Label(9, (i + 1) / looptime, String.valueOf(average(single_node_relay_best_path, i-looptime+1, i)));
                    sheet.addCell(label);
                    label = new Label(10, (i + 1) / looptime, String.valueOf(average(our_not_optimal_scheme, i-looptime+1, i)));
                    sheet.addCell(label);
                    label = new Label(11, (i + 1) / looptime, String.valueOf(average(our_scheme, i-looptime+1, i)));
                    sheet.addCell(label);
                    label = new Label(12, (i + 1) / looptime, String.valueOf(average(greedy_scheme, i-looptime+1, i)));
                    sheet.addCell(label);

                }
            }

            workbook.write();
            workbook.close();

        } catch (IOException e) {
            e.printStackTrace();
        } catch (WriteException e) {
            e.printStackTrace();
        }
    }
}
