----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 10/05/2025 04:46:37 PM
-- Design Name: 
-- Module Name: d_trigger_async_preset_tb - Behavioral
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

entity d_trigger_async_preset_tb is
--  Port ( );
end d_trigger_async_preset_tb;

architecture Behavioral of d_trigger_async_preset_tb is

    signal D, E, PRE : std_logic := '0';
    signal Q, nQ : std_logic;
    constant PERIOD : time := 20 ns;
    
begin
    DUT: entity work.d_trigger_async_preset port map(D=>D, E=>E, PRE=>PRE, Q=>Q, nQ=>nQ);

    clk: process
    begin
        while true loop E <= '0'; wait for PERIOD/2; E <= '1'; wait for PERIOD/2; end loop;
    end process;

    stim: process
    begin
        D <= '0'; wait for 10 ns;
        PRE <= '1'; wait for 10 ns; PRE <= '0'; wait for 30 ns;
        assert false report "Finished" severity failure;
    end process;
end Behavioral;
