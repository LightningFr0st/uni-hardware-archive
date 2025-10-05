----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 10/05/2025 03:58:01 PM
-- Design Name: 
-- Module Name: bistable_elem_tb - Behavioral
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

entity bistable_elem_tb is
--  Port ( );
end bistable_elem_tb;

architecture Behavioral of bistable_elem_tb is

component bistable_elem is
    Port ( nQ : out STD_LOGIC;
           Q : out STD_LOGIC);
end component;

signal Q, nQ : STD_LOGIC;
    
begin
    DUT: bistable_elem
        port map (
            Q  => Q,
            nQ => nQ
        );

    stim_proc: process
    begin
        wait for 100 ns;
        assert false report "Simulation finished" severity failure;
    end process;
end Behavioral;
