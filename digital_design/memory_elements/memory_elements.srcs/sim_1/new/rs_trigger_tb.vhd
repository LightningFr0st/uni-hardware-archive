----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 10/05/2025 04:53:45 PM
-- Design Name: 
-- Module Name: rs_trigger_tb - Behavioral
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

entity rs_trigger_tb is
--  Port ( );
end rs_trigger_tb;

architecture Behavioral of rs_trigger_tb is
signal R, S, C : std_logic := '0';
    signal Q, nQ : std_logic;
    constant PERIOD : time := 20 ns;
begin
    DUT: entity work.rs_trigger port map(R=>R, S=>S, C=>C, Q=>Q, nQ=>nQ);

    clk: process
    begin
        while true loop C <= '0'; wait for PERIOD/2; C <= '1'; wait for PERIOD/2; end loop;
    end process;

    stim: process
    begin
        
        S <= '1'; R <= '0'; wait for 25 ns; S <= '0'; wait for 10 ns;
        
        R <= '1'; S <= '0'; wait for 25 ns; R <= '0'; wait for 10 ns;
        
        R <= '1'; S <= '1'; wait for 25 ns; R <= '0'; S <= '0'; wait for 10 ns;
        assert false report "Finished" severity failure;
    end process;
end Behavioral;
