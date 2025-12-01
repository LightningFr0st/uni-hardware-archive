----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 11/30/2025 04:11:49 PM
-- Design Name: 
-- Module Name: test_experiment - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity test_experiment is
    Port (
        CLK       : in  STD_LOGIC;
        Reset     : in  STD_LOGIC;
        Load      : in  STD_LOGIC;              
        Seed      : in  STD_LOGIC_VECTOR(2 downto 0);
        Signature : out STD_LOGIC_VECTOR(0 to 2)
    );
end test_experiment;

architecture Structural of test_experiment is

signal gen_vec  : STD_LOGIC_VECTOR(2 downto 0);
signal dut_out  : STD_LOGIC;                    
signal sig_int  : STD_LOGIC_VECTOR(0 to 2);    

begin

    gen_inst: entity work.lfsr_external_beh
        port map (
            DOut  => open,    
            Q     => gen_vec,
            CLK   => CLK,
            Reset => Reset,
            Load  => Load,
            Seed  => Seed
        );

    dut_inst: entity work.dut_comb
        port map (
            X => gen_vec,  
            Y => dut_out
        );

    scsa_inst: entity work.scsa_beh
        port map (
            CLK       => CLK,
            Reset     => Reset,
            En        => '1',        
            DIn       => dut_out,     
            Load      => '0',         
            Seed      => "000",       
            Signature => sig_int
        );

    Signature <= sig_int;

end Structural;
