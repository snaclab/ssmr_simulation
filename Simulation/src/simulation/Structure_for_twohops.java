/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package simulation_for_case1;

/**
 *
 * @author chung
 */
public class Structure_for_twohops{
    private final double first, second;
    
        Structure_for_twohops(double first , double second){
            this.first = first;
            this.second = second;
        }
    
        public double get_first(){
            return this.first;
        }
        public double get_second(){
            return this.second;
        }
    }
