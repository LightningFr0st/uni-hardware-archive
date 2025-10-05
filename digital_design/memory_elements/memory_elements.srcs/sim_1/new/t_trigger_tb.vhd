----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 10/05/2025 04:55:16 PM
-- Design Name: 
-- Module Name: t_trigger_tb - Behavioral
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

entity t_trigger_tb is
--  Port ( );
end t_trigger_tb;

architecture Behavioral of t_trigger_tb is

signal D, C : std_logic := '0';
    signal Q : std_logic;
    constant PERIOD : time := 20 ns;
begin
    DUT: entity work.t_trigger port map(D=>D, C=>C, Q=>Q);

    clk: process
    begin
        while true loop C <= '0'; wait for PERIOD/2; C <= '1'; wait for PERIOD/2; end loop;
    end process;

    stim: process
    begin
        D <= '1'; wait for 100 ns;
        D <= '0'; wait for 40 ns;
        assert false report "Finished" severity failure;
    end process;
end Behavioral;
